# include "file.h"
# include "global.h"
# include "path.h"

struct file_ {
  HANDLE file;
};

void slashtail_clear (us *bufh, int lens, int *ofbl_noslash) {
  int id;

  for (id = lens - 1; id != -1; id--) {
    if (isslash (bufh[id]) != 0) {
      if (ofbl_noslash)
        *ofbl_noslash = id + 1;
      return ;
    }
  }
  if (ofbl_noslash)
    *ofbl_noslash = 0;
}

void slashclr_getpath (us *bufh, int lens, int *ofbl_noslash) {
  int id;

  for (id = lens - 1; id != -1; id--) {
    if (isslash (bufh[id]) == 0) {
      if (ofbl_noslash)
        *ofbl_noslash = id;
      return ;
    }
  }
  if (ofbl_noslash)
    *ofbl_noslash = 0;
}

int file_open (struct file_ **file_, uconst us *fname, int open_type, int access) {
  DWORD dipos_mask;
  DWORD access_mask;
  HANDLE win32fd;
  int s = -1;
  switch (open_type) {
  case FILE2_EXIST: dipos_mask = OPEN_EXISTING; break;
  case FILE2_EXIST_ORCREATE_CLEAR: dipos_mask = CREATE_ALWAYS; break;
  default : assert (0);
  }
  switch (access) {
  case FILE2_READ: access_mask = FILE_ATTRIBUTE_READONLY; break;
  case FILE2_READ_WRITE: access_mask = FILE_ATTRIBUTE_NORMAL; break;
  default : assert (0);
  }
  if (file_ != null) {
    * file_ = null;
    if ((win32fd = CreateFileW (fname,   
      GENERIC_READ | ((access == FILE2_READ)?0:GENERIC_WRITE),
      0, 
      NULL,   
      dipos_mask,   
      access_mask, 
      NULL))  != INVALID_HANDLE_VALUE) {
        struct file_ *e = umalloc (sizeof (struct file_));
        e->file = win32fd;
        * file_ = e;
        s = 0;
        * file_ = e;
    } else win32_thread_local_err = GetLastError ();
  }
  return s;
}

void file_close (struct file_ **file_) {
  if (file_ != null) {
    struct file_ *e = *file_;
    *file_ = null;
    if (e != null) {
      if (e->file != null)
        CloseHandle (e->file);
      ufree (e);
    }
  }
}

int file_flush (struct file_ *file_) {
  int s = -1;
  if (file_ != null) {
    if (FlushFileBuffers (file_->file) == FALSE)
      win32_thread_local_err = GetLastError ();
    else s = 0;
  }
  return s;
}

int file_del (uconst us *name) {
  int s = -1;
  if (DeleteFileW (name) == FALSE)
    win32_thread_local_err = GetLastError ();
  else s = 0;
  return s;
}

int file_exist (uconst us *name) {
  int s = 0;
  struct file_ *ef;
  if (file_open (& ef, name, FILE2_EXIST, FILE2_READ) == 0)
    file_close (& ef);
  else s = -1;
  return s;
}

int file_size (struct file_ *file_, int *rev) {  
  int s = -1;
  if (file_ != null) {
    DWORD e = GetFileSize (file_->file, NULL);
    if (e == INVALID_SET_FILE_POINTER)
      win32_thread_local_err = GetLastError ();
    else * rev = e, s = 0;
  }
  return s;
}

int file_setpos (struct file_ *file_, int pos) {
  int s = -1;
  if (pos == -1)
    return  0;
  else if (file_ != null) {
    if (((DWORD)SetFilePointer (file_->file, pos, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
      win32_thread_local_err = GetLastError ();
    else s = 0;
  }
  return s;
}

int file_lastmod (struct file_ *file_, uint64_t *rev) {
  int s = -1;
  if (file_ != null) {
    * rev = 0;
    if (GetFileTime (file_->file, NULL, NULL, (LPFILETIME)rev) == FALSE)
      win32_thread_local_err = GetLastError ();
    else s = 0;
  }
  return s;
}

int file_lastmod9 (uconst us *name, int64_t *recv) {
  int s = 0;
  WIN32_FILE_ATTRIBUTE_DATA win32_fd;
  if (0 != GetFileAttributesExW (name, GetFileExInfoStandard, & win32_fd))
    if (recv != null)
    * recv = * (uint64_t* ) & win32_fd.ftLastWriteTime;
    else ;
  else s = -1;
  return s;
}

int file_read (struct file_ *file_, int pos, void *buf, int size, int *nums_opr) {
  int s = -1;
  if (file_ != null) {
    DWORD f;
    if (pos != -1)
      if (file_setpos (file_, pos) != 0)
        return -1;
      else ;
    else ;
    if (ReadFile (file_->file, buf, size, & f, NULL) == FALSE)
      win32_thread_local_err = GetLastError ();
    else {
      if (nums_opr != null)
        *nums_opr = f; 
      s = 0;
    }
  }
  return s;
}

int file_write (struct file_ *file_, int pos, void *buf, int size, int *nums_opw) {
  int s = -1;
  if (file_ != null) {
    DWORD f;
    if (pos != -1)
      if (file_setpos (file_, pos) != 0)
        return -1;
      else ;
    else ;
    if (WriteFile (file_->file, buf, size, & f, NULL) == FALSE)
      win32_thread_local_err = GetLastError ();
    else {
      if (nums_opw != null)
        *nums_opw = f; 
      s = 0;
    }
  }
  return s;
}

#  if !defined (_SIMD_CRC32_SUPPORT)

const 
unsigned int crc32_lut[256] = {

  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
  0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
  0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
  0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
  0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
  0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
  0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
  0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
  0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
  0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
  0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
  0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
  0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
  0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
  0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
  0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
  0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
  0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
  0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
  0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
  0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
  0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
  0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
  0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
  0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
  0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
  0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
  0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
  0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
  0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
  0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
  0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
  0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
  0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
  0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
  0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
  0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
  0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
  0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
  0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
  0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
  0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
  0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

#endif 

int32_t 
crc32_get (int32_t init, void *buf, int nums) {

  unsigned int crc = ~init;
  unsigned char *cptr = buf;

  /* calculate CRC */
  while (nums --)
#  if !defined (_SIMD_CRC32_SUPPORT)
    /* this part for poor hareward's user. */
    crc = crc32_lut[(crc ^ *cptr++) & 0xFF] ^ (crc >> 8);
#  else 
    crc = _mm_crc32_u8 (crc, *cptr++);
#  endif
  /* final, neg the output value */
  return ~crc;
}

LIST_TYPE2_(uchar) 
set_serach (uconst us *path, uconst us *sfix, kbool *harm) {

  WIN32_FIND_DATAW win32_fd;
  HANDLE handle;

  us fdbuf[MAX_PATH];
  LIST_TYPE2_(uchar) s = null; 
  int id;

  kbool edot;
  us *sfix_ = sfix;
  /* make serach desc */
  assert (path != null);
  slashtail_clear (path, wcslen (path), & id);

  if (id == 0)  {
    if (harm)
     * harm = ktrue;
    return null;
  }
  wcsncpy_s (& fdbuf[0], MAX_PATH, path, id);
  wcscat_s (& fdbuf[0], MAX_PATH, _GR("\\*"));

  if (sfix != null && !wcscmp (sfix, _GR("nil")))
    sfix_ = null;
  if (harm) 
   * harm = kfalse;

  if (sfix_ != null) { 
    if ( *sfix_  == _GC('.')) {
      edot = ktrue;
      wcscat_s (& fdbuf[0], MAX_PATH, sfix_);
    } else {
      edot = kfalse;
      wcscat_s (& fdbuf[0], MAX_PATH, _GR("."));
      wcscat_s (& fdbuf[0], MAX_PATH, sfix_);
    }
  } 

  if ((handle = FindFirstFileW (
             & fdbuf[0], & win32_fd)) == INVALID_HANDLE_VALUE) {
    if (GetLastError () == ERROR_FILE_NOT_FOUND)
      return null;
    else   if (harm) {
      * harm = ktrue;
      return null;
    } assert (0);
  }

  while (ktrue) {
    struct uchar *q;
    us * po;
    int last_dotpos = -1;
    if (win32_fd.cFileName[0] == _GC('.')
      || (sfix_ == null 
      && (wcsrchr (& win32_fd.cFileName[0], _GC('.')) != null)))
      achieve L0;
    else if ( ! (win32_fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE
      || (win32_fd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)) )
      achieve L0;
    if (s == null)
      list_init(& s);
    uchar_init (& q);

    if (sfix_ != null) {
      po = wcsrchr (& win32_fd.cFileName[0], _GC('.'));
      last_dotpos = ( ( (uintptr_t) po) - ((uintptr_t) & win32_fd.cFileName[0]) ) / sizeof (us);
      uchar_assign( q, & win32_fd.cFileName[0], last_dotpos);
    } else {
      uchar_assign2( q, & win32_fd.cFileName[0]);
    }
    list_insert_tail (s, q);
    L0:
    if (FindNextFileW (handle, & win32_fd) == FALSE) {
      if (GetLastError () == ERROR_NO_MORE_FILES) {
        FindClose (handle);
        return s;
      } else ;  assert (0);
    }
  }
}



int file_write2 (struct file_ *file_, int pos, uconst us *format, ...)  {

  if (file_ != null) {
    us wtempbuf[2048];
    int vnums ;
    va_list args ;
    va_start (args, format) ;
    vnums = wvsprintfW (wtempbuf, format, args);
    va_end(args);
    return file_write (file_, pos, wtempbuf, vnums*sizeof (us), null );
  }
  return  -1;
}

int file_write3 (struct file_ *file_, int pos, const unsigned char *format, ...) {
  if (file_ != null) {
    unsigned char tempbuf[2048];
    int vnums ;
    va_list args ;
    va_start (args, format) ;
    vnums = wvsprintfA ( (LPSTR)tempbuf, (LPCSTR) format, args);
    va_end(args);
    return file_write (file_, pos, tempbuf, vnums, null );
  }
  return  -1;
}

#if 0
int main (void) {
  struct file_ *ef; 
  int s;
  int q;
  char c[20];
  file_open (& ef, L"D:\\eztry\\listg.h", FILE2_EXIST);
  file_size (ef, & s);
  file_read (ef, -1, c, 20, & q);
  file_setpos (ef, 1);
  file_close(& ef);
  return 0;
}
#endif 

