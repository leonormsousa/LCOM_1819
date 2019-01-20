#include "videoCard.h"

unsigned int h_res;
unsigned v_res;
unsigned int bits_per_pixel;
void* init_adress;
void* second_buffer;

#define BIOS_INTERFACE 0x10
#define VBE_MODE_SET 0x4F02
#define VBE_Mode_Info 0x4F01

/** Initiates the graphic mode with the settings of the mode passed as argument
 * Returns the address to the video memory mapped
 */
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

/** Enters in graphic mode
 */
int (vg_enter)(uint16_t mode)
{
  struct reg86u reg86;
  memset(&reg86, 0 , sizeof(reg86));
  reg86.u.b.intno=BIOS_INTERFACE;
  reg86.u.w.ax=VBE_MODE_SET;
  reg86.u.w.bx= (1<<14) | mode;
  if( sys_int86(&reg86) != OK ) 
     return 1;
  return 0; 
}

/** Maps the video memory necessary to the specified mode
 */
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
  init_adress = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if(init_adress == MAP_FAILED) 
    panic("couldn't map video memory");
  second_buffer = malloc (vram_size);
  return init_adress;
}

// int (vg_exit)()
// {
// 	struct reg86u reg86;
// 	reg86.u.b.intno = BIOS_INTERFACE;
// 	reg86.u.b.ah = 0x00;   
// 	reg86.u.b.al = 0x03;   
// 	if(second_buffer)
// 		free(second_buffer);
// 	if( sys_int86(&reg86) != OK) 
// 		return 1;
// 	return 0;
// }

/** Places on the vbe_mode_info_t struct the information/settings relative to the mode passed as argument
 */
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
