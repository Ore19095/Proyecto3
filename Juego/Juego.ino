#include "ili9341.h";


//--------------- CONSTANTES Y VARIABLES PARA LA PELOTA -------------------------
#define ANCHO_PELOTA 18
#define ALTO_PELOTA 18
int yPelota = 182;
int xPelota = 189;
int contadorPelota = 0; //para determinar que sprite hay que usar
int velocidadx = 0;
int velocidady = 0;

//---------- CONSTANTES Y VARIABLES PARA LA PALETA -------------------------
#define ANCHO_P_PEQUENA 36
#define ALTO_P_PEQUENA 20
int yPaleta = 200;
int xPaleta = 180;
int paletaVx = 0;
int paletaVy = 0;
int tamanioPaleta = 0;
///------------ VARIABLES PARA LOS BLOQUES -----------------
#define nBloques 2
#define ALTO_BLOQUE 12
#define ANCHO_BLOQUE 26
int xBloques [] = {85,110,135,160,185,210,
                   85,110,135,160,185,210,
                   85,110,135,160,185,210,
                   85,110,135,160,185,210};
int yBloques [] = {40,40,40,40,40,40,
                   65,65,65,65,65,65,
                   90,90,90,90,90,90,
                   115,115,115,115,115,115};
int mostrarBloque[] = {1,1,1,1,1,1,
                       1,1,1,1,1,1,
                       1,1,1,1,1,1,
                       1,1,1,1,1,1};
boolean yaCambio = false; //esta variable establece que si ya cambio de direccion una vez en una colisión ya no lo hará de nuevo
//-------- PINES DEL POTENCIOMETRO ------------------------
#define POTX PE_0
#define POTY PE_2
#define BUTTON PA_6
#define TIEMPO 5 //tiempo en milisegundos
int lecturaButton = 1;
int lecturaAnteriorEstable = 1;
int buttonAnterior = 1;
unsigned long timeButton = 0;
// -------VARIABLES DEL CONTROL DEL JUEGO ---------------------
boolean inicioJuego = false; // para senialar que la pelota se lanzó
boolean jugando = false; // para indicar que se esta iniciando la partida
int vidas = 3; // numero de vidas restantes
int score = 0; // puntaje del juego
int lvl = 1;
///char caracter1,caracter2,caracter3; // iniciales para guardar puntajes 
int contadorLetra = 0; // para observar que letra se va a usar
String letra = "" ;//para el nombre
const char letras[26]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  //SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  pinMode(PA_6, INPUT_PULLUP);
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
  readButton();
  if (lecturaAnteriorEstable == 0 && lecturaButton ==1 && !jugando){
    jugando = true;
    LCD_Clear(  0xFFFF);
    LCD_Print("3" , 150, 110, 2, 0x0, 0xFFFF);
    delay(1000);
    LCD_Print("2" , 150, 110, 2, 0x0, 0xFFFF);
    delay(1000);
    LCD_Print("1" , 150, 110, 2, 0x0, 0xFFFF);
    delay(1000);
    LCD_Print("inicio" , 120, 110, 2, 0x0, 0xFFFF);
    delay(1000);
  }
  // si se presiona el boton al estar en el menu de inicio se comienza el juego
  if(jugando) game();
  else{
    LCD_Print("Presione el boton" , 20, 80, 2, 0x0, 0xFFFF);
    LCD_Print(" para comenzar" , 20, 100, 2, 0x0, 0xFFFF);
  }
  
  //delay(10);
}
// funcion que se ejecuta cuando se esta jugando
void game(){
  
  LCD_Sprite(0,0, 16, 16, corazon,1, 0,0,0);
  LCD_Print("X" + String(vidas) , 18, 0, 2, 0x0, 0xFFFF);
  LCD_Print("Score:" + String(score) , 70, 0, 2, 0x0, 0xFFFF);
   LCD_Print("lvl." + String(lvl) , 220, 0, 2, 0x0, 0xFFFF);
  H_line(0,18,340,0x0);
 
  while(vidas > 0){
    readButton();
    if (lecturaAnteriorEstable == 0 && lecturaButton ==1 && !inicioJuego){
      inicioJuego = true;
      velocidadx = lvl/2+1;
      velocidady = -lvl/2-1; 
  }
  
    pintarPelota();
    moverPaleta();
    dibujarBloques();
    colisionDectection();
    comprobacionGanar();  
  }

  // se guarda el puntaje 
  LCD_Print("      Fin del juego" , 70, 80, 1.5, 0x0, 0xFFFF);
  LCD_Print("desea guardar su puntaje?", 70, 100, 1.5, 0x0, 0xFFFF);
  LCD_Print("      *Si   No"  ,70,120 , 1.5, 0x0, 0xFFFF);
  int opcion = 1;
  boolean seleccionado = false;

  int analogicoAnterior = 2048, actual= 2048; // para verificar que se este selecionando bien
  while(!seleccionado){
    analogicoAnterior = actual;
    delay(50);
    actual = analogRead(POTX);
    
    if( analogicoAnterior < 1000 && actual > 1024 && actual < 2500){
      LCD_Print("      *Si   No"  ,70,120 , 1.5, 0x0, 0xFFFF);
      opcion = 1;
    }
    else if( analogicoAnterior > 2600 && actual > 1024 && actual < 2500){
     LCD_Print("       Si  *No"  ,70,120 , 1.5, 0x0, 0xFFFF);
      opcion = 0;
    }
    readButton();
    if(lecturaAnteriorEstable == 0 && lecturaButton ==1) seleccionado = true;
    
  }

  //si si se quiere guardar el puntaje
  seleccionado = false;
  if(opcion  ==1 ){
    char letra1,letra2,letra3;
    LCD_Print("Seleccione sus iniciales"  ,70,100 , 1.5, 0x0, 0xFFFF);
    LCD_Print("         A _ _"  ,70,120 , 1.5, 0x0, 0xFFFF);
    seleccionado = false;
  
    while(!seleccionado){
      analogicoAnterior = actual;
      delay(50);
      actual = analogRead(POTX);
      
      if( analogicoAnterior > 2600 && actual > 1024 && actual < 2500){
        contadorLetra = (contadorLetra+1)%26;
        LCD_Print("         "+String(letras[contadorLetra])+ " _ _"  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      else if( analogicoAnterior < 1000 && actual > 1024 && actual < 2500){
        if(contadorLetra == 0) contadorLetra = 26; //se hace underflow
        contadorLetra = (contadorLetra-1)%26;
        LCD_Print("         "+String(letras[contadorLetra])+ " _ _"  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      readButton();
      if(lecturaAnteriorEstable == 0 && lecturaButton ==1) seleccionado = true;
      
    }
    letra1 = letras[contadorLetra];
    contadorLetra =0;
    LCD_Print("         "+String(letra1)+" A _"  ,70,120 , 1.5, 0x0, 0xFFFF);
    seleccionado = false;
    while(!seleccionado){
      analogicoAnterior = actual;
      delay(50);
      actual = analogRead(POTX);
      
      if( analogicoAnterior > 2600 && actual > 1024 && actual < 2500){
        contadorLetra = (contadorLetra+1)%26;
        LCD_Print("         "+ String(letra1)+" "+ String(letras[contadorLetra])+ " _"  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      else if( analogicoAnterior < 1000 && actual > 1024 && actual < 2500){
        if(contadorLetra == 0) contadorLetra = 26; //se hace underflow
        contadorLetra = (contadorLetra-1)%26;
        LCD_Print("         "+String(letra1)+" "+ String(letras[contadorLetra])+ " _"  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      readButton();
      if(lecturaAnteriorEstable == 0 && lecturaButton ==1) seleccionado = true;
      
    }
    letra2 = letras[contadorLetra];
    contadorLetra =0;
    LCD_Print("         "+String(letra1)+" "+String(letra2)+ " A"  ,70,120 , 1.5, 0x0, 0xFFFF);
    seleccionado = false;
    while(!seleccionado){
      analogicoAnterior = actual;
      delay(50);
      actual = analogRead(POTX);
      
      if( analogicoAnterior > 2600 && actual > 1024 && actual < 2500){
        contadorLetra = (contadorLetra+1)%26;
        LCD_Print("         "+ String(letra1)+" "+ String(letra2)+ " "+String(letras[contadorLetra])  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      else if( analogicoAnterior < 1000 && actual > 1024 && actual < 2500){
        if(contadorLetra == 0) contadorLetra = 26; //se hace underflow
        contadorLetra = (contadorLetra-1)%26;
        LCD_Print("         "+String(letra1)+" "+ String(letra2)+ " "+String(letras[contadorLetra])  ,70,120 , 1.5, 0x0, 0xFFFF);
      }
      readButton();
      if(lecturaAnteriorEstable == 0 && lecturaButton ==1) seleccionado = true;
      
    
    }
    letra3 = letras[contadorLetra];
    
    // se concatenan los 3 caracteres elegidos
    letra.concat(letra1);
    letra.concat(letra2);
    letra.concat(letra3);
    
    
  }
  
  
  
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

  //Serial.println(analogica1);
  //Serial.println(analogica2);

  if ( analogica1 <1500 | analogica1>2100){
    paletaVx = map(analogica1,0,4095,-10,10); 
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
  
  /*if (yPaleta + paletaVy >= 0 && yPaleta + paletaVy <= 240 - alto){
    yPaleta += paletaVy;
    if(!inicioJuego) xPelota += paletaVy
  }*/
  
  

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
  int ancho = 0;
  int alto = 0;

  switch(tamanioPaleta){
    case 0:
      alto = ALTO_P_PEQUENA;
      ancho = ANCHO_P_PEQUENA;
      break;
  }
  if( xPelota + velocidadx <0 || xPelota + velocidadx > 302) velocidadx*= -1;
  if( yPelota + velocidady <19 || (yPelota + velocidady) > 222) velocidady*= -1;

  xPelota += velocidadx;
  yPelota += velocidady;
  // si no se ha lanzado la pelota 
  if(!inicioJuego && xPaleta + paletaVx >= 0 && xPaleta + paletaVx <= 320 - ancho) xPelota += paletaVx;
    
  
  LCD_Sprite(xPelota, yPelota, ANCHO_PELOTA, ALTO_PELOTA, pelotaV,4,(contadorPelota*3)%4 ,0,0);
  /* se limpia el movimiento que hizo con la paleta si no se ha lanzado */
  if( !inicioJuego && xPaleta + paletaVx >= 0 && xPaleta + paletaVx <= 320 - ancho){
    if(paletaVx > 0) FillRect(xPelota - paletaVx -1, yPelota, paletaVx,ALTO_PELOTA, 0xFFFF);
    else FillRect(xPelota + ANCHO_PELOTA + 1 , yPelota, -paletaVx, ALTO_PELOTA, 0xFFFF);  
  }
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

void dibujarBloques(){
  for (int i = 0; i < nBloques ; i++){
    if(mostrarBloque[i] == 1 ) LCD_Sprite(xBloques[i],yBloques[i], ANCHO_BLOQUE, ALTO_BLOQUE, bloqueNaranja,4, 0,0,0);
  }
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

     // verificar colision con bloques 
     for (int i = 0; i < nBloques; i++){

        if((xPelota + ANCHO_PELOTA >= xBloques[i]  && xPelota <= xBloques[i] + ANCHO_BLOQUE) &&
            ( yBloques[i]  <= yPelota + ALTO_PELOTA && yBloques[i] + ALTO_BLOQUE > yPelota)&& mostrarBloque[i] == 1 ){
              if(xPelota + ANCHO_PELOTA <= xBloques[i] + ANCHO_BLOQUE/2 && xPelota + ANCHO_PELOTA >= xBloques[i] && velocidadx >= 0 ) velocidadx *= -1;
              else if (xPelota  >= xBloques[i] + ANCHO_BLOQUE/2 && xPelota  <= xBloques[i] + ANCHO_BLOQUE && velocidadx <= 0 ) velocidadx *= -1;
              if(!yaCambio){
                
                velocidady *= -1;
                yaCambio = true;
              }
              
              mostrarBloque[i] = 0;
              FillRect( xBloques[i], yBloques[i], ANCHO_BLOQUE, ALTO_BLOQUE, 0xFFFF);
              score += 1;
              LCD_Print("Score:" + String(score) , 70, 0, 2, 0x0, 0xFFFF);
            }
      
     }
     yaCambio = false; // preparando parael siguiente ciclo
    //verificar colision con la paleta 
     
    if( (xPelota + ANCHO_PELOTA >= xPaleta  && xPelota <= xPaleta + ancho) && ( yPaleta  <= yPelota + ALTO_PELOTA && yPaleta + alto > yPelota)){

        if(xPelota + ANCHO_PELOTA <= xPaleta + ancho/2 && xPelota + ANCHO_PELOTA >= xPaleta && velocidadx >= 0 ) velocidadx *= -1;
        else if (xPelota  >= xPaleta + ancho/2 && xPelota  <= xPaleta+ ancho && velocidadx <= 0 ) velocidadx *= -1;
        
        if ( velocidady > 0)  velocidady *= -1;
    }

    if( yPelota + velocidady + ANCHO_PELOTA >= 240 ) {
      vidas--;
      FillRect(xPelota, yPelota, ANCHO_PELOTA, ALTO_PELOTA, 0xFFFF);
      LCD_Print("X" + String(vidas) , 18, 0, 2, 0x0, 0xFFFF);
      velocidadx = 0;
      velocidady = 0;
      xPelota = xPaleta+9; // se localiza en en el centro de la paleta
      yPelota = yPaleta - ALTO_PELOTA;
      inicioJuego = false;
    } 
}

void readButton(){
    int lectura = digitalRead(BUTTON);

    if (lectura !=buttonAnterior) timeButton = millis();

    if (millis() - timeButton > TIEMPO ){
      lecturaAnteriorEstable = lecturaButton;
      lecturaButton = lectura;
    }

    buttonAnterior = lectura;   
}

// funcion que realiza la comprobación de que gano el nivel

void comprobacionGanar(){
    int variable= 0;
    for (int i = 0; i<nBloques; i++) variable+= mostrarBloque[i];

    if (variable == 0){
      FillRect(xPelota,yPelota, ANCHO_PELOTA,ALTO_PELOTA, 0xFFFF);
      lvl++;
      LCD_Print("lvl." + String(lvl) , 220, 0, 2, 0x0, 0xFFFF);
      velocidadx = 0;
      velocidady = 0;
      inicioJuego = false;
      xPelota = xPaleta+9; // se localiza en en el centro de la paleta
      yPelota = yPaleta - ALTO_PELOTA;
      pintarPelota();
      LCD_Print("Has pasado al" , 18, 50, 2, 0x0, 0xFFFF);
      LCD_Print("siguiente nivel" , 18, 70, 2, 0x0, 0xFFFF);
      LCD_Print("Presiona el boton" , 18, 90, 2, 0x0, 0xFFFF);
      LCD_Print("para continuar" , 18, 110, 2, 0x0, 0xFFFF);
      boolean presiono = false;
      while(!presiono ){
        readButton();
        if(lecturaAnteriorEstable == 0 && lecturaButton ==1) presiono = true;
      }
      FillRect(18,50,280 ,80, 0xFFFF);

      for(int i = 0; i< nBloques; i++) mostrarBloque[i] = 1;
       
    }

    
}
