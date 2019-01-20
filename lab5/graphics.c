#include "graphics.h"

void *video_mem;
unsigned int h_res;
unsigned v_res;
unsigned int bits_per_pixel;
void* init_adress;
char mystr[256];

#define BIOS_INTERFACE 0x10
#define VBE_MODE_SET 0x4F02
#define VBE_Controller_Info 0x4F00
#define VBE_Mode_Info 0x4F01

void *(vg_init)(uint16_t mode)
{
  lm_init(false);
  init_adress= map_adress(mode);
  if(init_adress==NULL)
    return NULL;
  if(vg_enter(mode)!= 0)
    return NULL;  
  return init_adress;
}

int (vg_enter)(uint16_t mode)
{
  struct reg86u reg86;
  memset(&reg86, 0 , sizeof(reg86));
  reg86.u.b.intno=BIOS_INTERFACE;
  reg86.u.w.ax=VBE_MODE_SET;
  reg86.u.w.bx= (1<<14) | mode;
  if( sys_int86(&reg86) != OK ) 
  {
     printf("\t vg_enter(): sys_int86() failed \n");
     return 1;
  }
  return 0; 
}

void *(map_adress)(uint16_t mode)
{
  vbe_mode_info_t info;
  vbe_get_mode_inf(mode, &info);

  h_res = info.XResolution;
  v_res = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;

  int r; 
  struct minix_mem_range mr; 
  unsigned int vram_base = info.PhysBasePtr; /* VRAM's physical addresss */ 
  unsigned int vram_size = h_res * v_res * ((bits_per_pixel+7)/8); 
  mr.mr_base = (phys_bytes) vram_base; 
  mr.mr_limit = mr.mr_base + vram_size; 

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r); 
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if(video_mem == MAP_FAILED) 
    panic("couldn't map video memory");
  return video_mem;
}

int (vg_draw_hline)(uint16_t	x, uint16_t  y, uint16_t len, uint32_t color)
{
   unsigned int bytes_per_pixel=(bits_per_pixel+7)/8;
    char* ptr=init_adress;
    ptr+= y*h_res*(bytes_per_pixel);
    ptr+= x*(bytes_per_pixel);
  for(int i=0;i<len;i++)
  {
    for(unsigned int j=0; j<bytes_per_pixel;j++)
    {
      uint8_t color_byte= (uint8_t)(color>>(j*8));
      *ptr=color_byte;
      ptr++;
    }
  }  
  return 0;
}

int (vg_draw_rectangle)(uint16_t	x, uint16_t  y, uint16_t width, uint16_t height, uint32_t color)
{
  for(int i=1; i<=height;i++)
  {
    vg_draw_hline(x, y, width, color);
    y++;
  }
  return 0;
}
char *convert_pointer(char *ptr)
{
  uint16_t segment, offset;
  segment=((uint32_t)(ptr) >>16) & 0x0FFFF;
  offset= (uint32_t)(ptr) & 0x0000FFFF;
  ptr=(char*)((((segment)<<4)& 0xF0000) | (offset & 0x0FFFF));
  return ptr;
}

int vbe_config_contr_info(vg_vbe_contr_info_t *info)
{
  char* init_adress= lm_init(false);
 #pragma pack(1)
 typedef struct  {
   char VbeSignature[4];            
   uint8_t VbeVersion[2];                
   char* OemStringPtr;            
   uint8_t Capabilities[4];
   uint16_t* VideoModePtr;        
   uint16_t TotalMemory;       
   uint16_t OemSoftwareRev;     
   char* OemVendorNamePtr;
   char* OemProductNamePtr;
   char* OemProductRevPtr;
   uint8_t Reserved[222];
   uint8_t OemData[256];  
} VbeInfoBlock;
#pragma options align = reset

  unsigned long size = sizeof(VbeInfoBlock);
  mmap_t map;
  
  struct reg86u reg86;
  memset(&reg86, 0 , sizeof(reg86));
  VbeInfoBlock *a =lm_alloc(size, &map);
  (*a).VbeSignature[0]= 'V';
  (*a).VbeSignature[1]= 'B';
  (*a).VbeSignature[2]= 'E';
  (*a).VbeSignature[3]= '2';

  reg86.u.b.intno=BIOS_INTERFACE;
  reg86.u.w.ax=VBE_Controller_Info;
  reg86.u.w.es = PB2BASE(map.phys);    
  reg86.u.w.di = PB2OFF(map.phys); 
 
  if( sys_int86(&reg86) != OK ) 
  {
     printf("\t vbe_config_contr_info(): sys_int86() failed \n");
    lm_free(&map);
     return 1;
  }
  a = (VbeInfoBlock*)map.virt;
  info->VBESignature[0]=(*a).VbeSignature[0];
  info->VBESignature[1]=(*a).VbeSignature[1];
  info->VBESignature[2]=(*a).VbeSignature[2];
  info->VBESignature[3]=(*a).VbeSignature[3];

  info->VBEVersion[0]=(*a).VbeVersion[0];
  info->VBEVersion[1]=(*a).VbeVersion[1];

  char *ptr;
  ptr=(*a).OemStringPtr;

  info->OEMString=convert_pointer(ptr) + (uint32_t)init_adress;
  
  ptr=(char*)(*a).VideoModePtr;
  info->VideoModeList=(uint16_t*)(convert_pointer(ptr) +(uint32_t)init_adress);
  
  info->TotalMemory= (*a).TotalMemory*64;
  
  ptr=(*a).OemVendorNamePtr;
  info->OEMVendorNamePtr=convert_pointer(ptr) + (uint32_t)init_adress;

  ptr=(*a).OemProductNamePtr;
  info->OEMProductNamePtr=convert_pointer(ptr) + (uint32_t)init_adress;
 
  ptr=(*a).OemProductRevPtr;
  info->OEMProductRevPtr=convert_pointer(ptr) + (uint32_t)init_adress;
  
  lm_free(&map);
  return 0; 
}

int vbe_get_mode_inf(u_int16_t mode, vbe_mode_info_t *info)
{
  lm_init(false);
  unsigned long size = sizeof(vbe_mode_info_t);
  mmap_t map;
  lm_alloc(size, &map);
  struct reg86u reg86;
  memset(&reg86, 0 , sizeof(reg86));
  reg86.u.b.intno=BIOS_INTERFACE;
  reg86.u.w.ax=VBE_Mode_Info;
  reg86.u.w.es = PB2BASE(map.phys);    
  reg86.u.w.di = PB2OFF(map.phys); 
  reg86.u.w.cx = mode; 
  if( sys_int86(&reg86) != OK ) 
  {
     printf("\t vbe_get_mode_inf(): sys_int86() failed \n");
    lm_free(&map);
     return 1;
  }
  *info = * (vbe_mode_info_t*) map.virt;
  lm_free(&map);
  return 0; 
}
