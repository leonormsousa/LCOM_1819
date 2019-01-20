#include "utility.h"


/** Returns the minimum of the two values passed as argument
 */
int min(int valor1, int valor2)
{
	if (valor1<valor2)
		return valor1;
	else
		return valor2;
}

/** Returns the maximum of the two values passed as argument
 */
int max(int valor1, int valor2)
{
	if (valor1>valor2)
		return valor1;
	else
		return valor2;
}

/** Converts the scan code passed as argument to the corresponding image (loaded xpm)
 */
unsigned char* scancode_to_img(uint32_t scan_code)
{
  switch(scan_code)
  {
    case 0x1e:
      return image_load(A_xpm);
    case 0x30:
      return image_load(B_xpm);
    case 0x2e:
      return image_load(C_xpm);
    case 0x20:
      return image_load(D_xpm);
    case 0x12:
      return image_load(E_xpm);
    case 0x21:
		  return image_load(F_xpm);
    case 0x22:
      return image_load(G_xpm);
    case 0x23:
      return image_load(H_xpm);
    case 0x17:
      return image_load(I_xpm);
    case 0x24:
      return image_load(J_xpm);
    case 0x25:
      return image_load(K_xpm);
    case 0x26:
      return image_load(L_xpm);
    case 0x32:
      return image_load(M_xpm);
    case 0x31:
      return image_load(N_xpm);
    case 0x18:
      return image_load(O_xpm);
    case 0x19:
      return image_load(P_xpm);
    case 0x10:
      return image_load(Q_xpm);
    case 0x13:
      return image_load(R_xpm);
    case 0x1F:
      return image_load(S_xpm);
    case 0x14:
      return image_load(T_xpm);
    case 0x16:
      return image_load(U_xpm);
    case 0x2F:
      return image_load(V_xpm);
    case 0x11:
      return image_load(W_xpm);
    case 0x2D:
      return image_load(X_xpm);
    case 0x15:
      return image_load(Y_xpm);
    case 0x2C:
      return image_load(Z_xpm);
    case 0x02:
      return image_load(um_xpm);
    case 0x03:
      return image_load(dois_xpm);
    case 0x04:
      return image_load(tres_xpm);
    case 0x05:
      return image_load(quatro_xpm);
    case 0x06:
      return image_load(cinco_xpm);
    case 0x07:
      return image_load(seis_xpm);
    case 0x08:
      return image_load(sete_xpm);
    case 0x09:
      return image_load(oito_xpm);
    case 0x0A:
      return image_load(nove_xpm);
    case 0x0B:
      return image_load(zero_xpm);
    default:
      return NULL;
  }
}

/** Converts the integer passed as argument to the corresponding image (loaded xpm)
 */
unsigned char* int_to_img(int value)
{
  switch(value%10)
  {
    case 0:
      return image_load(zero_xpm);
    case 1:
      return image_load(um_xpm);
    case 2:
      return image_load(dois_xpm);
    case 3:
      return image_load(tres_xpm);
    case 4:
      return image_load(quatro_xpm);
    case 5:
      return image_load(cinco_xpm);
    case 6:
      return image_load(seis_xpm);
    case 7:
      return image_load(sete_xpm);
    case 8:
      return image_load(oito_xpm);
    case 9:
      return image_load(nove_xpm);
    default:
      return image_load(empty_xpm);
  }
}
