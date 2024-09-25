#include "memory.hpp"

struct FindProcessContext
{
  OsInstance<> *os;
  const char *name;
  ProcessInstance<> *target_process;
  bool found;
};

bool find_process(struct FindProcessContext *find_context, Address addr)
{

  if (find_context->found)
  {
    return false;
  }

  if (find_context->os->process_by_address(addr,
                                           find_context->target_process))
  {
    return true;
  }

  const struct ProcessInfo *info = find_context->target_process->info();

  if (!strcmp(info->name, find_context->name))
  {
    // abort iteration
    find_context->found = true;
    return false;
  }

  // continue iteration
  return true;
}

// Credits: learn_more, stevemk14ebr
size_t findPattern(const PBYTE rangeStart, size_t len, const char *pattern)
{
  size_t l = strlen(pattern);
  PBYTE patt_base = static_cast<PBYTE>(malloc(l >> 1));
  PBYTE msk_base = static_cast<PBYTE>(malloc(l >> 1));
  PBYTE pat = patt_base;
  PBYTE msk = msk_base;
  if (pat && msk)
  {
    l = 0;
    while (*pattern)
    {
      if (*pattern == ' ')
        pattern++;
      if (!*pattern)
        break;
      if (*(PBYTE)pattern == (BYTE)'\?')
      {
        *pat++ = 0;
        *msk++ = '?';
        pattern += ((*(PWORD)pattern == (WORD)'\?\?') ? 2 : 1);
      }
      else
      {
        *pat++ = getByte(pattern);
        *msk++ = 'x';
        pattern += 2;
      }
      l++;
    }
    *msk = 0;
    pat = patt_base;
    msk = msk_base;
    for (size_t n = 0; n < (len - l); ++n)
    {
      if (isMatch(rangeStart + n, patt_base, msk_base))
      {
        free(patt_base);
        free(msk_base);
        return n;
      }
    }
    free(patt_base);
    free(msk_base);
  }
  return -1;
}

uint64_t Memory::get_proc_baseaddr() { return proc.baseaddr; }

process_status Memory::get_proc_status() { return status; }

void Memory::check_proc()
{
  if (status == process_status::FOUND_READY)
  {
    short c;
    Read<short>(proc.baseaddr, c);

    if (c != 0x5A4D)
    {
      status = process_status::FOUND_NO_ACCESS;
      close_proc();
    }
  }
}

Memory::Memory() { mf_log_init(LevelFilter::LevelFilter_Info); }

int Memory::open_os()
{
  // load all available plugins
  if (inventory)
  {
    mf_inventory_free(inventory);
    inventory = nullptr;
  }
  inventory = mf_inventory_scan();
  if (!inventory)
  {
    mf_log_error("unable to create inventory");
    return 1;
  }
  printf("inventory initialized: %p\n", inventory);

  const char *conn_name = "kvm";
  const char *conn_arg = "";

  const char *conn2_name = "qemu";
  const char *conn2_arg = "";

  const char *os_name = "win32";
  const char *os_arg = "";

  ConnectorInstance connector;
  conn = &connector;

  // initialize the connector plugin
  if (conn)
  {
    printf("Using %s connector.\n", conn_name);
    if (mf_inventory_create_connector(inventory, conn_name, conn_arg,
                                      &connector))
    {
      printf("Unable to initialize %s connector.\n", conn_name);
      printf("Fallback to %s connector.\n", conn2_name);

      if (mf_inventory_create_connector(inventory, conn2_name, conn2_arg,
                                        &connector))
      {
        printf("Unable to initialize %s connector.\n", conn2_name);
        return 1;
      }
    }

    printf("Connector initialized: %p\n",
           connector.container.instance.instance);
  }

  // initialize the OS plugin
  if (mf_inventory_create_os(inventory, os_name, os_arg, conn, &os))
  {
    printf("unable to initialize OS\n");
    return 1;
  }

  printf("os plugin initialized: %p\n", os.container.instance.instance);
  return 0;
}

const std::string filename = "DTB.txt";
bool check_exist()
{
  std::ifstream file(filename);
  if (!file)
  {
    printf("DTB File does not exist.\n");
    return false;
  }
  return true;
}

std::set<size_t> load_valid_dtbs()
{
  std::set<size_t> dtb_set;
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line))
  {
    std::istringstream iss(line);
    size_t dtb;
    if (iss >> dtb)
    {
      dtb_set.insert(dtb);
    }
  }
  return dtb_set;
}

void append_valid_dtb(size_t dtb)
{
  std::ofstream file(filename, std::ios::app);
  file << dtb << std::endl;
}

int Memory::open_proc(const char *name)
{
  int ret;
  const char *target_proc = name;
  bool exist_dtb_file = check_exist();
  std::set<size_t> valid_dtbs = load_valid_dtbs();

  if (!(ret = os.process_by_name(CSliceRef<uint8_t>(target_proc),
                                 &proc.hProcess)))
  {
    const struct ProcessInfo *info = proc.hProcess.info();

    printf("%s process found: 0x%lx] %d %s %s\n", target_proc, info->address,
           info->pid, info->name, info->path);
    const short MZ_HEADER = 0x5A4D;
    char *base_section = new char[8];
    long *base_section_value = (long *)base_section;
    memset(base_section, 0, 8);
    CSliceMut<uint8_t> slice(base_section, 8);
    os.read_raw_into(proc.hProcess.info()->address + 0x520, slice); // win10
    proc.baseaddr = *base_section_value;

    bool found_valid_dtb = false;
    if (exist_dtb_file)
    {
      for (size_t dtb : valid_dtbs)
      {
        proc.hProcess.set_dtb(dtb, Address_INVALID);
        short header;
        Read<short>(*base_section_value, header);
        if (header == MZ_HEADER)
        {
          printf("Using valid DTB from file: %zu\n", dtb);
          found_valid_dtb = true;
          break;
        }
      }
    }
    if (!found_valid_dtb)
    {
      printf("Searching for a new DTB...\n");
      for (size_t dtb = 0; dtb < SIZE_MAX; dtb += 4096)
      {
        proc.hProcess.set_dtb(dtb, Address_INVALID);
        short header;
        Read<short>(*base_section_value, header);
        if (header == MZ_HEADER)
        {
          printf("Found new DTB: %zu\n", dtb);
          append_valid_dtb(dtb); // 追加新的 DTB 到文件中
          found_valid_dtb = true;
          break;
        }
      }
    }
    if (!found_valid_dtb)
    {
      printf("Failed to find valid DTB for process %s\n", name);
      status = process_status::FOUND_NO_ACCESS;
      return ret;
    }
    status = process_status::FOUND_READY;
  }
  else
  {
    status = process_status::NOT_FOUND;
  }

  return ret;
}

Memory::~Memory()
{
  if (inventory)
  {
    mf_inventory_free(inventory);
    inventory = nullptr;
    mf_log_info("inventory freed");
  }
}

void Memory::close_proc()
{
  proc.baseaddr = 0;
  status = process_status::NOT_FOUND;
}

uint64_t Memory::ScanPointer(uint64_t ptr_address, const uint32_t offsets[],
                             int level)
{
  if (!ptr_address)
    return 0;

  uint64_t lvl = ptr_address;

  for (int i = 0; i < level; i++)
  {
    if (!Read<uint64_t>(lvl, lvl) || !lvl)
      return 0;
    lvl += offsets[i];
  }

  return lvl;
}