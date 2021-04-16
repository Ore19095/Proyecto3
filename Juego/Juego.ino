#include "ili9341.h";


//--------------- CONSTANTES PARA LOS SPRITES -------------------------

#define ANCHO_PELOTA 18
#define ALTO_PELOTA 18
//----------- VARIABLES PARA LOS SPRITES ------------------------------
int yPelota = 35;
int xPelota = 35;
int contadorPelota = 0; //para determinar que sprite hay que usar
int velocidadx = 2;
int velocidady = 1;

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  LCD_Clear(  0xFFFF);
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  
  
}


void pintarPelota(){
  
  if( xPelota + velocidadx <0 || xPelota + velocidadx > 302) velocidadx*= -1;
   if( yPelota + velocidady <0 || (yPelota + velocidady) > 222) velocidady*= -1;

  xPelota += velocidadx;
  yPelota += velocidady;

  
  delay(16);
  
  LCD_Sprite(xPelota, yPelota, ANCHO_PELOTA, ALTO_PELOTA, pelotaV,4,(contadorPelota*3)%4 ,0,0);
  
   if(velocidadx >0 && velocidady<0){
     V_line(xPelota - velocidadx, yPelota, ANCHO_PELOTA, 0XFFFF);
     H_line(xPelota, yPelota+ALTO_PELOTA, ANCHO_PELOTA, 0XFFFF);
  }else if( velocidadx>0 && velocidady>0){
    V_line(xPelota -velocidadx, yPelota, ANCHO_PELOTA, 0XFFFF);
  }
  
  contadorPelota++;
}
