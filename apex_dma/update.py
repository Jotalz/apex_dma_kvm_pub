import re
import configparser
from datetime import datetime
import sys


class ConsoleColors:
    RESET = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    PURPLE = '\033[95m'
    CYAN = '\033[96m'


unrecognizedLines = []
notFindLines = []
current_date = datetime.now()
date = current_date.strftime("%Y/%m/%d")


def update_offsets(offset_h_path, offset_ini_path):

    offset_pattern = re.compile(r'0x[\dA-Fa-f]+')  # 匹配 0x...
    date_pattern = re.compile(r'updated (\d{1,4}/\d{1,4}/\d{1,4})')  # 匹配日期
    # 读取 offset.ini 中的偏移量
    dumpFile = configparser.ConfigParser(strict=False)
    dumpFile.read(offset_ini_path)

    # 读取 offset.h 文件内容
    with open(offset_h_path, 'r') as headFile:
        offset_h_content = headFile.read()

    # 按行拆分文本
    lines = offset_h_content.split('\n')

    for i in range(len(lines)):
        keywords = re.findall(r'//\s*(\S+)', lines[i])
        if keywords and re.match(r"\[", keywords[0]):
            comment_pattern = re.compile(r'\[(.+?)\]\.(.+)')  # 匹配出 [xxx].xxx
            comment_match = comment_pattern.search(keywords[0])
            section, keyword = comment_match.group(1), comment_match.group(2)
            if section in dumpFile and keyword in dumpFile[section]:
                value = dumpFile[section][keyword]  # 从dump文件中找出偏移
                lines[i] = re.sub(offset_pattern, value, lines[i], count=1)
                lines[i] = re.sub(date_pattern, "updated "+date, lines[i], count=1)
            else:
                notFindLines.append(lines[i])
        elif not keywords:
            pass
        elif keywords[0] == "Date":
            lines[i] = f"//Date {date}"
        elif keywords[0] == "GameVersion":
            lines[i] = f"//GameVersion = {dumpFile['Miscellaneous']['GameVersion']}"
        else:
            if lines[i]:
                unrecognizedLines.append(lines[i])
    # 将更新后的内容写回文件
    updated_content = '\n'.join(lines)
    with open(offset_h_path, 'w') as headFile:
        headFile.write(updated_content)

    print(ConsoleColors.GREEN + "Update completed!" + ConsoleColors.RESET)
    if notFindLines is not None or unrecognizedLines is not None:
        print(ConsoleColors.RED + "Some offsets need to be updated manually" + ConsoleColors.RESET)
    if notFindLines:
        print(ConsoleColors.RED + "NotFindLines Lines:")
        for line in notFindLines:
            print(line)
        print(ConsoleColors.RESET,end="")

    if unrecognizedLines:
        print(ConsoleColors.YELLOW + "Unrecognized Lines:")
        for line in unrecognizedLines:
            print(ConsoleColors.YELLOW + line + ConsoleColors.RESET)


if __name__ == '__main__':
    args = sys.argv
    if len(sys.argv) >= 3:
        offset_h_path = sys.argv[1]
        offset_ini_path = sys.argv[2]
        update_offsets(offset_h_path, offset_ini_path)
    else:
        print("Requires two parameters.\neg.>py update.py offsets.h offsets.ini")

