#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int (vg_enter)(uint16_t mode);

void *(map_adress)(uint16_t mode);

int vbe_config_contr_info(vg_vbe_contr_info_t *info);

int vbe_get_mode_inf(u_int16_t mode, vbe_mode_info_t *info);
