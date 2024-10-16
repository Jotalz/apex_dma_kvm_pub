hello-world = Hello, world!
menu-value-enabled = 启用
menu-value-disabled = 禁用
main-menu-title = 主菜单 GameVersion-v3.0.82.42
title-main-setting = 主题设置 ------ 0
title-loot-setting = 物资设置 ------ 4
title-player-setting = 玩家热能设置 ------ 8
title-aim-setting = 自瞄设置 ------ 15
title-extra-skill = 额外技能 ------ 27
title-other-setting = 其他设置 ------ 32
menu-item-firing-range = 射击场模式
menu-item-keyboard = 键鼠模式
menu-item-gamepad = 手柄模式
menu-item-item-glow = 物品发光
menu-item-player-glow = 玩家热能
menu-item-player-glow-dist = 玩家热能距离
menu-item-smooth-value = 平滑
menu-item-smooth-sub-value = 副键平滑
menu-item-weapon-predict = 武器预测微调
input-prompt-smooth-value = 瞄准平滑值 (50~500):
input-prompt-smooth-sub-value = 副键平滑值 (高优先级):
info-invalid-smooth-value = 输入无效。
    平滑值可设置范围是 50 到 500
menu-item-change-bone-aim = 瞄准部位设置
menu-value-bone-auto = 自动选择
menu-value-bone-nearest = 首次击中
menu-value-bone-head = 头部
menu-value-bone-neck = 颈部
menu-value-bone-chest = 胸部
menu-value-bone-gut-shut = 腹部
menu-value-bone-unknown = 未知
input-prompt-bone-value = 
    设置新的瞄准部位:
        x => 自动
        h => 首次击中
        0 => 头部
        1 => 颈部
        2 => 胸部
        3 => 腹部
info-invalid-bone-value = 输入无效。
    瞄准部位可设置值: x, 0, 1, 2, 3
info-invalid-value = 输入无效。
menu-item-loot-glow-filled = 物品发光填充
menu-item-player-glow-filled = 玩家热能填充
menu-item-player-outline-size = 热能轮廓尺寸
input-prompt-player-outlines = 热能轮廓半径 (0~255):
input-prompt-player-distance = 玩家热能距离 (10~500):
info-player-outline-updated = "热能轮廓半径" 已更新至: {$value}
info-invalid-distance = 输入无效。
    "玩家热能距离" 只能介于 0 到 500 之间。
info-invalid-outline-size = 输入无效。
    "热能轮廓大小" 只能介于 0 到 255 之间。
menu-item-update-glow-colors = 热能颜色设置
menu-item-change-ads-fov = 开镜FOV
input-prompt-ads-fov = 设置 "开镜自瞄FOV" 值:
    (1~50)
info-invalid-ads-fov = 输入无效。
    "开镜自瞄FOV" 只能介于 1.0 到 50.0 之间。
menu-item-change-non-ads-fov = 腰射FOV
input-prompt-non-ads-fov = 设置 "腰射自瞄FOV" 值: 
    (1~50)
info-invalid-non-ads-fov = 输入无效。
    "腰射自瞄FOV" 只能介于 1.0 到 50.0 之间。
menu-item-change-flick-fov = 瞬击FOV
input-prompt-flick-fov = 设置 "瞬击自瞄FOV" 值: 
    (5~50)
info-invalid-flick-fov = 输入无效。
    "瞬击自瞄FOV" 只能介于 5.0 到 50.0 之间。
menu-item-flick-nearest = 瞬击最近位置
menu-item-super-glide = 自动SG
menu-item-item-filter-settings = 物品透视设置
menu-item-hotkey-settings = 热键设置
menu-item-death-boxes = 死亡盒子
menu-item-save-settings = 保存设置
menu-item-load-settings = 加载设置
info-saved = 已保存
info-failed = 失败
info-loaded = 已加载
info-fallback-config = 回退到默认配置
menu-item-toggle-nade-aim = 高抛雷
menu-value-no-nade-aim = 开镜瞄准
menu-value-nade-aim-on = 开镜取消
menu-item-toggle-onevone = 1v1
menu-item-toggle-no-recoil = 仅后座调节
menu-item-change-recoil-pitch = 垂直后坐力
menu-item-change-recoil-yaw = 水平后坐力
input-prompt-recoil-pitch = 
    设置新的垂直后坐力系数:
        (0 到 90) 
        0 => 不抵消垂直后坐力
input-prompt-recoil-yaw = 
    设置新的水平后坐力系数:
        (0 到 90) 
        0 => 不抵消水平后坐力
info-invalid-recoil = Invalid value.
    后坐力系数必须在0-90之间
menu-item-set-fps-predict = 预测FPS
menu-value-calc-fps = 自适应FPS
input-prompt-fps-predict = 
    设置 "预测FPS" 值:
        (0~500)
        0 => 自适应FPS
menu-item-player-armor-glow-color = 护甲热能颜色
menu-item-weapon-model-glow = 被观战武器发光
info-weapon-model-glow = 
    通常 => 不发光
    被队友观战时 => 绿色
    被他人观战时 => 蓝色->橙色->红色->彩色
                     <3    <5    <7    7+
menu-item-toggle-overlay = 覆盖层绘制
menu-value-no-overlay = 无覆盖
menu-value-external-overlay = 外部覆盖
info-expecting-value-count = 期望 3 个值，但得到 {$getting} 个！
info-cannot-parse-input-values = 该输入无法解析为值！
info-values-out-of-range = 值超出范围！
input-prompt-color-rgb = 
    输入 "{$item_label}" 的 RGB 值:
       (每个通道 0~1.0)
        例如
        1 0 0
        0.5 0.5 0.5
glow-color-menu-title = 发光颜色菜单
menu-item-glow-colors = 发光颜色 ({$item_label})
color-item-not-viz-target = 目标不可见
color-item-viz-target = 目标可见
color-item-knocked-target = 倒地目标
info-glow-colors-updated = 发光颜色"{$item_label}"已更新（R：{$r}，G：{$g}，B：{$b}）。
menu-item-back-to-main-menu = 返回主菜单
input-prompt-keycode = 
    为“{$item_label}”输入新值：
    （例如，108 表示鼠标左键）
hotkey-menu-title = 热键菜单
hotkey-item-aimbot1 = 自动瞄准热键1
hotkey-item-aimbot2 = 自瞄热键2
hotkey-item-trigger-bot = 自动扳机热键(绑定热键2)
hotkey-item-flick-bot = 瞬击热键
hotkey-item-quick-glow = 临时开关热能热键
hotkey-item-quick-aim = 临时开关自瞄热键
hotkey-item-algs-radar = ALGS雷达热键
menu-item-key-codes = 键码表
item-filter-menu-title = 物品选择菜单
info-invalid-keycode = 值无效。"{$item_label}"值必须介于 0 和 255 之间。
item-light-weapons = 轻型武器
item-heavy-weapons = 重型武器
item-energy-weapons = 能量武器
item-sniper-weapons = 狙击武器
item-armors = 护甲
item-healing = 治疗
item-nades = 手雷
item-backpacks = 背包
item-hop-ups = 特殊配件
item-scopes = 瞄准镜
red-is-disable = {"红色 = 未选择"}
greed-is-enabled = {"绿色 = 已选择"}
light-weapons-menu-title = 轻型武器菜单
light-weapons-section = 轻型武器:
weapon-p2020 = P2020手枪
weapon-re45 = RE-45 自动手枪
weapon-alternator = 转换者冲锋枪
weapon-r99 = R-99 冲锋枪
weapon-r301 = R-301 卡宾枪
weapon-m600 = M600 喷火轻机枪
weapon-g7-scout = G7侦查枪
loot-light-ammo = 轻型弹药
light-weapon-mags-section = 轻型弹匣:
loot-light-weapon-mag = 加长轻型弹匣
weapon-stocks-section = 枪托:
loot-standard-stock = 标准枪托
loot-sniper-stock = 狙击枪托
weapon-suppressors-section = 枪管:
loot-weapon-suppressors = 枪管稳定器
weapon-lasers-section = 激光瞄准镜:
loot-weapon-lasers = 激光瞄准镜
weapon-hop-ups-menu-title = 特殊配件菜单
weapon-hop-ups-section = 武器特殊配件:
loot-turbo-charger = 涡轮增压器(未开放)
loot-skull-piecer = 穿颅器(未开放)
loot-hammer-points = 锤击点
loot-disruptor-rounds = 干扰器(未开放)
loot-boosted-loader = 快速装填器
loot-anvil-receiver = 铁砧接收器(未开放)
loot-doubletap-trigger = 双发扳机(未开放)
loot-dual-shell = 双弹装填器(未开放)
loot-kinetic-feeder = 动能供弹器(未开放)
loot-quickdraw-holster = 快捷枪套(未开放)
heavy-weapons-menu-title = 重型武器菜单
heavy-weapons-section = 重型武器:
weapon-flatline = VK-47 平行步枪
weapon-hemlock = 赫姆洛克连发突击步枪 
weapon3030-repeater = 30-30 连发枪
weapon-rampage = 暴走轻机枪
weapon-prowler = 猎兽冲锋枪
weapon-car-smg = CAR冲锋枪
loot-heavy-ammo = 重型弹药
heavy-weapon-mags-section = 重型弹匣:
loot-heavy-weapon-mag = 加长重型弹匣
energy-weapons-menu-title = 能量武器菜单
energy-weapons-section = 能量武器:
weapon-l-star = L-STAR轻机枪
weapon-nemesis = 复仇女神突击步枪
weapon-havoc = 哈沃克步枪
weapon-deovtion = 专注轻机枪
weapon-triple-take = 三重式狙击枪
weapon-volt = 电能冲锋枪
loot-energy-ammo = 能量弹药
energy-weapon-mags-section = 能量弹匣:
loot-energy-weapon-mag = 加长能量弹匣
sniper-weapons-menu-title = 狙击武器菜单
sniper-weapons-section = 狙击武器:
weapon-wingman = 辅助手枪
weapon-longbow = 长弓精确步枪
weapon-charge-rifle = 充能步枪
weapon-sentinel = 哨兵狙击步枪
weapon-bow = 波赛克
weapon-kraber = 克雷贝尔
loot-sniper-ammo = 狙击弹药
sniper-weapon-mags-section = 狙击弹匣:
loot-sniper-weapon-mag = 加长狙击弹匣
armors-menu-title = 护甲菜单
armors-section = 护盾:
loot-body-shield = 护盾
loot-evo-shield = 进化护盾
helmets-section = 头盔:
loot-helmet = 头盔
knockdown-shields-section = 击倒护盾:
loot-knockdown-shield = 击倒护盾
healing-items-menu-title = 治疗补给菜单
healing-items-section = 治疗补给:
loot-accelerant = 绝活加速剂
loot-phoenix = 凤凰治疗包
loot-small-health = 注射器
loot-large-health = 医疗包
loot-small-shield-batt = 小型护盾电池
loot-large-shield-batt = 护盾电池
loot-mobile-respawn = 移动重生信标
nades-menu-title = 手雷菜单
nade-items-section = 手雷:
loot-frag-grenade = 碎片手雷
loot-arc-star = 电弧星
loot-thermite = 铝热剂手雷
backpacks-menu-title = 背包菜单
backpacks-section = 背包:
loot-light-backpack = 背包
loot-medium-backpack = 背包
loot-heavy-backpack = 背包
loot-gold-backpack = 背包
scopes-menu-title = 瞄准镜菜单
scopes-section = 瞄准镜:
loot1x-hcog = 单倍全息衍射式瞄准镜“经典”
loot2x-hcog = 2 倍全息衍射式瞄准镜“格斗家”
loot1x-holo = 单倍幻影
loot1x2x-holo = 单倍至 2 倍可调节式幻影瞄准镜
loot-optic-threat = 单倍数字化威胁
loot3x-hcog = 3 倍全息衍射式瞄准镜“游侠”
loot2x4x-aog = 2 倍至 4 倍可调节式高级光学瞄准镜
loot6x-sniper-optic = 6 倍狙击手
loot4x8x-sniper-optic = 4 倍至 8 倍可调节式狙击手
loot-sniper-threat = 6 倍至 12 倍数字化狙击威胁
key-codes-menu-title = 键码表:
keycode108-mouse1-left = 108 鼠标左键 (mouse1)
keycode109-mouse2-right = 109 鼠标右键 (mouse2)
keycode110-mouse3-middle = 110 鼠标中键 (mouse3)
keycode111-mouse4-side = 111 鼠标侧键 (mouse4)
keycode112-mouse5-side = 112 鼠标侧键 (mouse5)
menu-item-back-to-hotkey-menu = 返回热键菜单
weapons-predict-menu-title = 武器预测菜单
input-prompt-weapons-predict = 
    武器预测速度微调 (-0.5~0.5):
    数值越大，自瞄时提前量越多
    默认值是0.08
info-invalid-predict = 预测值必须在-0.5~0.5之间
menu-value-prefix = {" [当前: "}
menu-value-suffix = {"]"}
loot-level1-name = 等级1
loot-level2-name = 等级2
loot-level3-name = 等级3
loot-level4-name = 等级4
loot-level5-name = 等级5
menu-item-favorite-player-glow = 突出显示最爱的玩家
menu-item-kbd-backlight-ctrl = 键盘背光控制
menu-item-bow-charge-rifle-aim = 蓄能武器自瞄
menu-item-trigger-auto-shot = 扳机
menu-item-super-grpple = 超级钩
menu-item-auto-tapstrafe = 自动TS
