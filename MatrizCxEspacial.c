//definen las entradas del dip switch (5 para el control de las entradas y 3 para el control de las salidas) 
#define E1   GPIOA_IDR.B0
#define E2   GPIOA_IDR.B1
#define E3   GPIOA_IDR.B2
#define E4   GPIOA_IDR.B3
#define E5   GPIOA_IDR.B4
#define S1   GPIOA_IDR.B5
#define S2   GPIOA_IDR.B6
#define S3   GPIOA_IDR.B7
//definen las salidas, los leds representan el estado de la salida: ocupado-libre
#define L1   GPIOA_ODR.B8
#define L2   GPIOC_ODR.B7
#define L3   GPIOA_ODR.B10

// matrices de control
  int etapa1_mat_control[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
  int etapa2_mat_control[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
// arreglos de entrada/salida
  int entradas[5]={0,0,0,0,0}, salidas[3]={0,0,0};
  int c_entradas[5] = {0,0,0,0,0},c_salidas[3] = {0,0,0};
//leds
  int leds[3] = {0,0,0};
// matrices de etapa
  int etapa1_mat[3][6], etapa2_mat[3][4];
  
  int contador=0;
  int e,s,i,j;

void conf_maq(){
     //Se configuran salidas digitales
     GPIO_Digital_Output(&GPIOA_BASE,_GPIO_PINMASK_8|_GPIO_PINMASK_9|_GPIO_PINMASK_10);
     GPIO_Digital_Output(&GPIOC_BASE,_GPIO_PINMASK_7);

     //Se configuran las entradas digitales
     GPIO_Digital_Input(&GPIOA_BASE,_GPIO_PINMASK_0|_GPIO_PINMASK_1|_GPIO_PINMASK_2|_GPIO_PINMASK_3
     |_GPIO_PINMASK_4|_GPIO_PINMASK_5|_GPIO_PINMASK_6|_GPIO_PINMASK_7);
}

void main(){
  conf_maq();
  while(1){
    //relacion entrada-salida
    (E1 == 1)? (entradas[0]=1) : (entradas[0]=0);
    (E2 == 1)? (entradas[1]=1) : (entradas[1]=0);
    (E3 == 1)? (entradas[2]=1) : (entradas[2]=0);
    (E4 == 1)? (entradas[3]=1) : (entradas[3]=0);
    (E5 == 1)? (entradas[4]=1) : (entradas[4]=0);
    Delay_ms(1000);
    (S1 == 1)? (salidas[0]=1) : (salidas[0]=0);
    (S2 == 1)? (salidas[1]=1) : (salidas[1]=0);
    (S3 == 1)? (salidas[2]=1) : (salidas[2]=0);
    
    //detecta las relaciones entrada-salida
    for(e = 0; e<5; e++){
        if(entradas[e] == 1 && c_entradas[e] == 0){         //activa una entrada
            for (s = 0; s<3; s++){
                if(salidas[s] == 1 && c_salidas[s] == 0){   //la entrada activada detecta la relación con una salida
                    etapa1_mat_control[e][s] = 1;
                    etapa2_mat_control[s][s] = 1;
                    c_entradas[e] = 1;
                    c_salidas[s] = 1;
                }
            }
        }else if(entradas[e] == 0 && c_entradas[e] != 0){   //si se desactiva una entrada
            for (s = 0; s<3; s++){
                if(salidas[s] == 0 && c_salidas[s] != 0){
                    etapa1_mat_control[e][s] = 0;
                    etapa2_mat_control[s][s] = 0;
                    c_entradas[e] = 0;
                    c_salidas[s] = 0;
                }
            }
        }
    }

    //ETAPA DE CONTROL POR SALIDA
    for(i=0; i<3; i++){
        for(j=0; j<5; j++){
            etapa1_mat[i][j] = ((entradas[j] == 1 && etapa1_mat_control[j][i] == 1)? 1 : 0);
            contador+=etapa1_mat[i][j];
        }
        etapa1_mat[i][5] = ((contador>0)? 1 : 0);
        contador=0;
    }
    //ETAPA DE CONTROL POR ENTRADA
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            etapa2_mat[i][j] = ((etapa1_mat[j][5] == 1 && etapa2_mat_control[i][j] == 1)? 1 : 0);
            contador+=etapa2_mat[i][j];
        }
        etapa2_mat[i][3] = ((contador>0)? 1 : 0);
        leds[i] = etapa2_mat[i][3];
        contador=0;
    }

    //Mostrando salidas en leds
    if(leds[0]) L1 = 1;
    else L1 = 0;
    if(leds[1]) L2 = 1;
    else L2 = 0;
    if(leds[2]) L3 = 1;
    else L3 = 0;
  }

}
