#include "ili9341.h";


//--------------- CONSTANTES Y VARIABLES PARA LA PELOTA -------------------------
#define ANCHO_PELOTA 18
#define ALTO_PELOTA 18
int yPelota = 0;
int xPelota = 0;
int contadorPelota = 0; //para determinar que sprite hay que usar
int velocidadx =1;
int velocidady = 1;

//---------- CONSTANTES Y VARIABLES PARA LA PALETA -------------------------
#define ANCHO_P_PEQUENA 36
#define ALTO_P_PEQUENA 20
int yPaleta = 200;
int xPaleta = 180;
int paletaVx = 0;
int paletaVy = 0;
int tamanioPaleta = 0;

//-------- PINES DEL POTENCIOMETRO ------------------------
#define POTX PE_0
#define POTY PE_2



//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  //LCD_Clear(0x00);
  LCD_Clear(  0xFFFF);
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {

  //delay(16);  
  
  pintarPelota();
  moverPaleta();
  colisionDectection();
  //delay(10);
}



void moverPaleta(){
  
  int ancho = 0;
  int alto = 0;

  switch(tamanioPaleta){
    case 0:
      alto = ALTO_P_PEQUENA;
      ancho = ANCHO_P_PEQUENA;
      break;
  }
  
  int analogica1 =analogRead(POTX);
  int analogica2 =analogRead(POTY);

  Serial.println(analogica1);
  Serial.println(analogica2);

  if ( analogica1 <1500 | analogica1>2100){
    paletaVx = map(analogica1,0,4095,-5,5); 
  }else {
    paletaVx = 0;
  }
/*  if ( analogica2 <1500 | analogica2>2100){
    paletaVy = map(analogica2,0,4095,-5,5);
  }else {
    paletaVy = 0;
  } no se movera en y */
  
  //paletaVx = map(analogica1,0,4096,-3,3); 
  //paletaVy = map(analogica2,0,4096,-3,3);
  if ( xPaleta + paletaVx >= 0 && xPaleta + paletaVx <= 320 - ancho) xPaleta += paletaVx;
  if (yPaleta + paletaVy >= 0 && yPaleta + paletaVy <= 240 - alto) yPaleta += paletaVy;
  
  

  LCD_Sprite( xPaleta, yPaleta , ANCHO_P_PEQUENA,ALTO_P_PEQUENA, plataformaAzulPequena ,1, 0 ,0,0);

  
  
  //--------------- ELIMINAR LOS RESTOS DEL SPRITE ANTERIOR --------------------------------
  if(paletaVx >=0 && paletaVy<=0){
     
     for (int i = 1; i <= paletaVx; i++) V_line(xPaleta - i,  yPaleta   - paletaVy , alto, 0XFFFF);
     for (int i = 0; i<= -paletaVy; i++)  H_line(xPaleta - paletaVx, yPaleta +i+ alto, ancho, 0XFFFF);
      
  }else if( paletaVx>=0 && paletaVy>=0){
    
    for (int i = 1; i <= paletaVx; i++) V_line(xPaleta - i, yPaleta - paletaVy, alto, 0XFFFF);
    for (int i = 1; i<= paletaVy; i++)  H_line(xPaleta-paletaVx, yPaleta-i, ancho, 0XFFFF);
    
  }else if (paletaVx<= 0 && paletaVy <= 0 ){
     for (int i = 0; i <= -paletaVx; i++) V_line(xPaleta + i + ancho,  yPaleta - paletaVy , alto, 0XFFFF);
     for (int i = 0; i<= -paletaVy; i++)  H_line(xPaleta - paletaVx, yPaleta + i + alto, ancho, 0XFFFF);
  }else{
    
    for (int i = 0; i <= -paletaVx; i++) V_line(xPaleta + i + ancho,  yPaleta   - paletaVy , alto, 0XFFFF);
    for (int i = 1; i<= paletaVy; i++)  H_line(xPaleta-paletaVx, yPaleta-i, ancho, 0XFFFF);
  }

  
}



void pintarPelota(){
  
  if( xPelota + velocidadx <0 || xPelota + velocidadx > 302) velocidadx*= -1;
  if( yPelota + velocidady <0 || (yPelota + velocidady) > 222) velocidady*= -1;

  xPelota += velocidadx;
  yPelota += velocidady;

  
  LCD_Sprite(xPelota, yPelota, ANCHO_PELOTA, ALTO_PELOTA, pelotaV,4,(contadorPelota*3)%4 ,0,0);
  //-- se limpia el rasto anterior del sprite anterior ---------------------------------
   if(velocidadx >=0 && velocidady<=0){
     
     for (int i = 1; i <= velocidadx; i++) V_line(xPelota - i,  yPelota   - velocidady , ALTO_PELOTA, 0XFFFF);
     for (int i = 0; i<= -velocidady; i++)  H_line(xPelota - velocidadx, yPelota +i+ ALTO_PELOTA, ANCHO_PELOTA, 0XFFFF);
      
  }else if( velocidadx>=0 && velocidady>=0){
    
    for (int i = 1; i <= velocidadx; i++) V_line(xPelota - i, yPelota - velocidady, ANCHO_PELOTA, 0XFFFF);
    for (int i = 1; i<= velocidady; i++)  H_line(xPelota-velocidadx, yPelota-i, ANCHO_PELOTA, 0XFFFF);
    
  }else if (velocidadx<= 0 && velocidady <= 0 ){
     for (int i = 0; i <= -velocidadx; i++) V_line(xPelota + i + ANCHO_PELOTA,  yPelota   - velocidady , ALTO_PELOTA, 0XFFFF);
     for (int i = 0; i<= -velocidady; i++)  H_line(xPelota - velocidadx, yPelota + i + ALTO_PELOTA, ANCHO_PELOTA, 0XFFFF);
  }else{
    
    for (int i = 0; i <= -velocidadx; i++) V_line(xPelota + i + ANCHO_PELOTA,  yPelota   - velocidady , ALTO_PELOTA, 0XFFFF);
    for (int i = 1; i<= velocidady; i++)  H_line(xPelota-velocidadx, yPelota-i, ANCHO_PELOTA, 0XFFFF);
  }
  
  
  contadorPelota++;
}


void colisionDectection(){
    int ancho = 0;
    int alto = 0;

    switch(tamanioPaleta){
      case 0:
        alto = ALTO_P_PEQUENA;
        ancho = ANCHO_P_PEQUENA;
        break;
    }
  
    //verificar colision con la paleta 
     
    if( (xPelota + ANCHO_PELOTA >= xPaleta  && xPelota <= xPaleta + ancho) && ( yPaleta  <= yPelota + ALTO_PELOTA && yPaleta + alto > yPelota)){

        if(xPelota + ANCHO_PELOTA <= xPaleta + ancho/2 && xPelota + ANCHO_PELOTA >= xPaleta && velocidadx >= 0 ) velocidadx *= -1;
        else if (xPelota  >= xPaleta + ancho/2 && xPelota  <= xPaleta+ ancho && velocidadx <= 0 ) velocidadx *= -1;
        
        if ( velocidady > 0)  velocidady *= -1;
    }

    if( yPelota + ANCHO_PELOTA == 240 ) {
      LCD_Print( "Game Over" , 20, 100, 2, 0x0528,  0xffff);
      velocidadx = 0;
      velocidady = 0;
    }
        
    
}
