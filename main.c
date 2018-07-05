#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define Rx 1200
#define Ry 630
#define RAIO 4
#define MAX_ARMAS 1000

void caminho( char **mat, int *x, int *y, int visao_x, int visao_y, int z, int vida );
void desvia( int inicio, int *y, int x, int tam, int dir, int z );
int andar( int inicio, int passo, int x, int y, int z, int vida );
int conta_armas( char **mat, int x, int y, int visao_x, int visao_y, int z );
void desenha_fundo( char **mat, BITMAP *temp, int x, int y, char c );
int calc_tamanho( char **mat, int coluna, int linha, int direcao );
char **copia_arquivo( char *nome, int *x, int *y );
void coloca_armas( char **mat, int *troca_mouse );
void menu_opcoes( char **mat, int *troca_mouse );
int preenche_tiros( char **matArq );
int creditos();
int aju_da();
int recor();
void desenha_tiro( int n_armas );
void ativa_tiros( int n_armas );
void zera_tiros( int n_armas );
int calcula_visao( int y );
void define_veloc( int n );
int compara_pontos();
void inicializa();
void contador();
void deinit();
void init();

typedef struct _sprite_{
        
        int x, y;
        float vida;
}sprite;

typedef struct _tiro_ {
        int x, y;
        int arma;
        int cont_x;
        int cont_y;
        int ativo;
}tiro;

BITMAP *buffer;
BITMAP *menu;
BITMAP *chao;
BITMAP *bmp;
BITMAP *arma1;
BITMAP *arma2;
BITMAP *arma3;
BITMAP *mouse;
BITMAP *base;
BITMAP *fim;
BITMAP *bala;
BITMAP *pista;
BITMAP *perdeu;
BITMAP *ajuda;
BITMAP *credito;
BITMAP *recordes;
BITMAP *menu_principal;
BITMAP *arvore;
BITMAP *menuarma1;
BITMAP *menuarma2;
BITMAP *menuarma3;
tiro **tiros;
sprite *zumbis;

/* Variáveis globais: */
int level;
int vidas;
int pontos;
int voltar = 0;
int n_zumbi = 8;
int cont_x[10];
int cont_y[10];
int aux[10]; /* Variável utilizada para trocar de contador: quando 0, cont_x e quando 1, cont_y */
int moedas;
int estatistica;
int veloc = 1000;

int main(){
    
    init();
    install_timer();
    inicializa();
    srand(time(NULL));
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    FILE *arq;
    
    int sair = 0, troca_mouse;
	int x, y, i, j, k;
	int sentido;
	int cont[10];
	int temp = 0, temp2 = 0;
	int tempx, tempy;
	int n_armas;
	char **matArq;
    
    tiros = (tiro **)malloc(MAX_ARMAS*sizeof(tiro *));
	
    for( i=0; i<MAX_ARMAS; i++ )
	     tiros[i] = (tiro *)malloc(n_zumbi*sizeof(tiro));
			
	matArq = (char **)malloc((Rx/30)*sizeof(char *));
    
    for( i=0; i<(Rx/30); i++ )
          matArq[i] = (char *)malloc((Ry/30)*sizeof(char));
    
    zumbis = (sprite *) malloc(n_zumbi*sizeof(sprite));
    
    for (i=0;i<MAX_ARMAS;i++)
        for( j=0; j<n_zumbi; j++ ){
	         tiros[i][j].ativo = 0;
	         tiros[i][j].arma = -1;
             tiros[i][j].cont_y = tiros[i][j].cont_x = 0;
          }
	         
	while( sair==0 ){
           
           /* Quando está no menu principal as posições dos zumbis ficam no estado incial */
           if( voltar==0 ){
               for( i=0; i<n_zumbi; i++ ){
                    zumbis[i].x = 1200;
                    zumbis[i].y = x = y = 0;
                    zumbis[i].vida = 500*level;
                    cont[i] = 0;
                    aux[i] = -1;    
               }
               estatistica = 0;
               vidas = 10;
               pontos = 0;
               moedas = 100;
               level = 1;
               /* O arquivo que contém os caracteres que representam o cenário e copiado para uma matriz */
               matArq = copia_arquivo( "arquivos/fundo1.txt", &x, &y );
               draw_sprite(buffer, menu_principal, 0, 0);
               draw_sprite(buffer, mouse, mouse_x, mouse_y);
               blit(buffer, screen, 0, 0, 0, 0, Rx , Ry);
               if( (mouse_b & 1) && (mouse_x > 580) && (mouse_x < 660) && (mouse_y < 331) && (mouse_y >266) ){
                   clear(buffer);
                   voltar = 1;
                   troca_mouse = -1;
               }
               if( (mouse_b & 1) && (mouse_x > 475) && (mouse_x < 687) && (mouse_y < 402) && (mouse_y >341) ){
                   clear(buffer);
                   voltar = recor();
                   troca_mouse = -1;
               }
               if( (mouse_b & 1) && (mouse_x > 495) && (mouse_x < 673) && (mouse_y < 484) && (mouse_y >411) ){
                   clear(buffer);
                   voltar = aju_da();
                   troca_mouse = -1;
               }
               if( (mouse_b & 1) && (mouse_x > 480) && (mouse_x < 687) && (mouse_y < 557) && (mouse_y >493 )){
                   clear(buffer);
                   voltar = creditos();
                   troca_mouse = -1;
               }
               if( (mouse_b & 1) && (mouse_x > 522) && (mouse_x < 649) && (mouse_y > 561 ) && (mouse_y < 608) )
                   sair = 1;
               }
            
           while( voltar==1 ){
                  
                  /* Chama a função responsável por desenhar o cenário a partir da matriz */
                  desenha_fundo( matArq, chao, x, y, '#' );
                  desenha_fundo( matArq, base, x, y, '.' );
                  desenha_fundo( matArq, pista, x, y ,'-');
                  desenha_fundo( matArq , arvore , x, y,'!');
                  desenha_fundo( matArq, fim, x, y, '*' );
                  desenha_fundo( matArq, arma1, x, y, '0' );
                  desenha_fundo( matArq, arma2, x, y, '1' );
                  desenha_fundo( matArq, arma3, x, y, '2' );
                  
                  draw_sprite( buffer, menu, Rx-300, 0 );
                  draw_sprite(buffer, mouse, mouse_x, mouse_y);
                  
                  menu_opcoes( matArq, &troca_mouse ); /* Função que permite interações com o menu lateral, de opções */
                  coloca_armas( matArq, &troca_mouse );
                  n_armas = preenche_tiros( matArq );
                  
                  for( i=0; i<n_zumbi; i++ ){
                       caminho( matArq, &zumbis[i].x, &zumbis[i].y, RAIO, RAIO, i, zumbis[i].vida ); /* Define o caminho de cada zumbi */
                       if( zumbis[i].vida <= 0 ){ /* Se a vida do zumbi correspondente chegar a zero */
                           cont[i]++; /* variável auxiliar */
                           aux[i] = -1; /* Zera os contadores do zumbi */
                           if( cont[i]<=1 ){ 
                               pontos += 10*level; /* Aumenta os pontos do jogador */
                               moedas += 3; /* e aumenta o número de moedas */
                               estatistica++; /* incrementa a variável que conta quantos zumbis estão inativos */
                           }
                       }
                       if( zumbis[i].x==840 && zumbis[i].y>=180 && zumbis[i].y<=390 ){ /* Se o zumbi atravessou o 'castelo' */
                           cont[i]++; /* variável auxiliar */
                           if( cont[i]<=1 ){
                               vidas--; /* diminui as vidas do jogador */
                               estatistica++;
                           }
                       }
                  }
                  textprintf_ex(buffer, font, 1000 , 40,makecol(255,255,255),-1, "%d " , moedas);  
                  textprintf_ex(buffer, font, 1000, 80, makecol(255,255,255), -1, "  PONTOS: %d  VIDAS: %d", pontos, vidas );
                  textprintf_ex(buffer, font, 1000, 100, makecol(255,255,255), -1, "LEVEL: %d ", level );
                  
                  /* Chama as funções de tiro das armas */
                  ativa_tiros(n_armas);
                  zera_tiros(n_armas);
                  desenha_tiro(n_armas);
                  
                  if( estatistica==n_zumbi ){ /* Se o número de zumbis inativos for igual ao numero de zumbis inicial */
                      temp++;
                      if( temp<=1 ){
                          for( i=0; i<n_zumbi; i++ ){
                               zumbis[i].x = 1200; /* 'Zera' as variáveis correspondentes à movimentação do zumbi */
                               zumbis[i].y = 0;
                               aux[i] = -1; /* zera os contadores */
                               cont[i] = 0;
                               zumbis[i].vida = 500+200*level; /* Aumenta a vida dos zumbis para próxima fase */
                          }
                          estatistica = 0; /* considera os zumbis ativos */
                          level++; /* aumenta o level */
                          define_veloc(50*level); /* aumenta a velocidade */
                          install_int(contador, veloc);
                          moedas += 25; /* o jogador ganha 25 moedas */
                          temp = 0;
                      }
                  }
                  if( vidas<=0 ){ /* Se o jogador perder todas as vidas */
                      if( temp==0 )
                          temp = compara_pontos(); /* verifica se sua pontuação ficou entre as cinco melhores */
                      clear(buffer);
                      draw_sprite( buffer, perdeu, 0, 0 ); /* exibe a imagem de fim de jogo */
                      textprintf_ex(buffer, font, 980, 80, makecol(255,255,255), -1, "PONTOS: %d ", pontos ); /* exibe a pontuação */
                      draw_sprite(buffer, mouse, mouse_x, mouse_y);
                      blit(buffer, screen, 0, 0, 0, 0, Rx , Ry);
                      
                      if( (mouse_b & 1) && (mouse_x > 347) && (mouse_y > 181 )&& (mouse_x < 830) && (mouse_y < 270 ) )
                          voltar = 0;
                  }
                  blit(buffer, screen, 0, 0, 0, 0, Rx , Ry); /* Desenha o buffer na tela */
                  clear(buffer); /* Limpa o buffer */
           }
    }
    free(zumbis); /* Libera a memória a alocada para o vetor */
    free(tiros); /* Libera a memória a alocada para a matriz */
	stop_midi();
    deinit(); /* Chama as funções de finalização do allegro */
	return 0;
}
END_OF_MAIN();

/* A função verifica q distância entre o zumbi e a arma e, dependendo dessa, ativa os tiros */
void ativa_tiros( int n_armas ){
     
     int i, j;
     
     for( i=0; i<n_armas; i++ )
          for( j=0; j<n_zumbi; j++ ){
               /* Se adestância for menor ou igual a três */
               if( (sqrt(pow((zumbis[j].x -tiros[i][j].x)/30, 2) + pow((zumbis[j].y -tiros[i][j].y)/30, 2)) <= 3)) 
                   /* e se na posição vetor de tiros contém o número de uma arma */
                   if ((tiros[i][j].arma == 0) || (tiros[i][j].arma == 1) || (tiros[i][j].arma == 2))
                      tiros[i][j].ativo = 1;  /* o tiro é ativado */
          }
}

void desenha_tiro( int n_armas ){
     
     int i, j, k, tempx, tempy;
     
     for ( i=0; i<n_armas; i++ ){
         for( k=0; k<n_zumbi; k++ ){
              /* Se os contadores das posições dos zumbis e tiverem ativados  */
              if( aux[k]!=-1 && ( tiros[i][k].ativo == 1 )){
                  switch( tiros[i][k].arma ){  /* A vida do zumbi é decrementada dependendo da arma */
                          case 0: 
                               if( zumbis[k].vida>0 )
                                   zumbis[k].vida = zumbis[k].vida - 0.5;
                               break;
                          case 1:
                               if( zumbis[k].vida>0 )
                                   zumbis[k].vida = zumbis[k].vida - 1;
                               break;
                          case 2:
                               if( zumbis[k].vida>0 )
                                   zumbis[k].vida = zumbis[k].vida - 2;
                               break;
                  }
                  /* Chama a função para determinar o sentido da bala a partir das posições do zumbi e da arma */
                  tempx = compara_posicoes( zumbis[k].x, tiros[i][k].x );
                  tempy = compara_posicoes( zumbis[k].y, tiros[i][k].y );
                  /* Incrementa os contadores pra movimentação do tiro */
                  tiros[i][k].cont_x++;
                  tiros[i][k].cont_y++;
                  /* Desenha os tiros no buffer */
                  draw_sprite( buffer, bala, tiros[i][k].x + tempx*tiros[i][k].cont_x, tiros[i][k].y + tempy*tiros[i][k].cont_y );
              }
         }
     }
}

int compara_posicoes( int n1, int n2 ){
    
    if( n1 > n2 )
        return 1;
    if( n1 < n2 )
        return -1;
    if( n1 == n2 )
        return 0;
}

void zera_tiros( int n_armas ){
     
     int i, j;
     
     for( i=0; i<n_armas; i++ ){
          for( j=0; j<n_zumbi; j++ ){
               if( tiros[i][j].cont_x == 40 ){
                   tiros[i][j].cont_x = tiros[i][j].ativo = 0; /* Zera os contadores das armas e desativa os tiros */
               }
               if( tiros[i][j].cont_y == 40 ){
                   tiros[i][j].cont_y = tiros[i][j].ativo = 0;
               }   
          }
     }    
}

/* A partir da matriz principal, preenche a matriz de tiros */
int preenche_tiros( char **matArq ){
    
    int i, j, k, n = 0;
    
    for ( i=0; i<Ry/30; i++ )
        for( j=0; j<(Rx-360)/30; j++ )
             for( k=0; k<n_zumbi; k++ ){
                  /* Percorre a matriz do cenário e quando um char representar uma arma, incrementa o número de armas
                     e a matriz de tiros é preenchida com a posição da arma, e um número correpondente */
                  if( matArq[i][j] == '0' ){
                      tiros[n][k].x = j*30;
                      tiros[n][k].y = i*30;
                      tiros[n][k].arma = 0;
                      n++;
                  }
                  if( matArq[i][j] == '1' ){
                      tiros[n][k].x = j*30;
                      tiros[n][k].y = i*30;
                      tiros[n][k].arma = 1;
                      n++;
                  }
                  if( matArq[i][j] == '2' ){
                      tiros[n][k].x = j*30;
                      tiros[n][k].y = i*30;
                      tiros[n][k].arma = 2;
                      n++;
                  }
             }
     return n; /* Retorna o número de armas */
}

void caminho( char **mat, int *x, int *y, int visao_x, int visao_y, int z, int vida ){
     
     int obst, i, j, dir;
     
     /* Se a posição do zumbi for menor que o cenário */
     if( *x < Rx-360 ){
         /* Se a próxima posição na matriz não for '#', ou seja, encontrou um obstáculo */
         if(( mat[*y/30][*x/30+1] != '#' ) && ( mat[*y/30][*x/30+1] != '-' ) && (mat[*y/30][*x/30+1] != '!')){
             if( *y-30*visao_y>30 && *y+30*visao_y<600 ){ /* Se a 'visão' do zumbi não ultrapassar os limetes verticais da tela */
                 
                 dir = conta_armas( mat, *x, *y, visao_x, visao_y, z ); /* Decide qual o melhor caminho a partir da quantidade de armas  */
             }
             else{ /* Se a 'visão' do zumbi ultrapassar os limetes verticais da tela */
                   /* Chama a função que encontra a melhor direção, mas passando como parâmetro a visão ajustada */
                   dir = conta_armas( mat, *x, *y, visao_x, calcula_visao( *y ), z );
             }
             obst = calc_tamanho( mat, *x, *y, dir ); /* Calcula o tamanho do ostáculo */
             desvia( 0, *(&y), *x, obst, dir, z ); /* anda horizontalmente o suficiente para desviar */
         }
         else{ /* Se a próxima posição na matriz for '#' */
              cont_y[z] = 0; /* zera o contador referente a movimentação vertical */
         }
         *x = andar( 0, 30, *x, *y, z, vida ); /* Movimenta o zumbi na horizontal */
     }
     if( *x==Rx-360 && (*y/30<8 || *y/30>14) ){ /* Se chegar no limete horizontal da tela */
         /* Anda na vertical, o suficiente para entrar no 'castelo' */
         if( *y/30<8 )
             dir = 1;
         if( *y/30>14 )
             dir = -1;
         desvia( 0, *(&y), *x, 8, dir, z );
     }
     else
         *x++;
}

/* Se tiver entre as cinco melhores pontuações, a pontuação do jogador é gravada */
int compara_pontos(){
     
     int i = 0 ,j;
     int k = 0;
     int *vet;

     FILE *arq = fopen("arquivos/score.txt", "r");
     
     vet = (int *)malloc(5*sizeof(int));
     
     if( arq!=NULL ){
         
         for( i=0; i<5; i++ ){ /* Copia as pontuações do arquivo para um vetor */
              fscanf( arq, "%d", &vet[i] );
         }
         fclose(arq); 
     }
     
     if( pontos>=vet[4] ){
         i = 0;
         
	     while( pontos<=vet[i]){ /* Procura a posição que a nova pontuação deverá ocupar */
                i++;
                  printf("i = %d \n", i);
         }
     
         for( j=4; j>i+1; j-- ){
              
              printf("j: %d ", vet[j]);
              vet[j] = vet[j-1]; /* Desloca as posições do vetor */
         }
    
         vet[i] = pontos; /* e insere a nova posição */
     
         arq = fopen( "arquivos/score.txt", "w"); /* Abri o arquivo */
     
         if( arq!=NULL ){
             for( i=0; i<5; i++ ){ /* e escreve o vetor no arquivo */
                  fprintf( arq, "%d ", vet[i]);
             }
         fclose(arq); 
         }
     }
     return 1;
}

/* Calcula quantas armas tem nas posições próximas a posição atual do zumbi */
int conta_armas( char **mat, int x, int y, int visao_x, int visao_y, int z ){
    
    int i, j, n_acima, n_abaixo;
    
    n_acima = n_abaixo = 0;
    
    for( i=0; i<visao_y; i++ )
         for( j=0; j<visao_x; j++ ){
              /* Se as próximas posições abaixo da posição atual for uma arma */
              if( mat[(y/30)+i][(x/30)+j] == '0' || mat[(y/30)+i][(x/30)+j] == '1' || mat[(y/30)+i][(x/30)+j] == '2' )
                  n_abaixo++;
              /* Se as próximas posições acima da posição atual for uma arma */
              if( mat[(y/30)-i][(x/30)+j] == '0' || mat[(y/30)-i][(x/30)+j] == '1' || mat[(y/30)-i][(x/30)+j] == '2' )
                  n_acima++;
         }
    if( n_acima<n_abaixo ) /* Retorna a melhor direção */
        return -1;
    else
        return 1;
}

/* Ajusta a visão a partir da posição y */
int calcula_visao( int y ){
     
     int i;
     
     if( y>315 )
         i = (600 - y)/30;
     else
         i = (30 + y)/30;
     
     return i;
}

void coloca_armas( char **mat, int *troca_mouse ){
     
     int i, testa = 0;
     /* Se o mouse não estiver na região do menu de opções e não for o mouse padrão */
     if( mouse_b & 1 && mouse_x<Rx-330 && *troca_mouse!=-1 ){
         
         mouse = load_bitmap("imagens/mouse.bmp",NULL); /* Carrega a imagem do mouse padrão */
         draw_sprite(buffer, mouse, mouse_x, mouse_y); /* Desenha o mouse no buffer */
         if( mat[(mouse_y)/30+1][(mouse_x)/30+1] == ' ' ){ /* Se na matriz a posição não estiver ocupada */
             switch( *troca_mouse ){ /* Dependendo do mouse coloca a respectiva arma na matriz e diminui a quantidade de moedas */
                     case 0:
                          mat[(mouse_y/30)+1][(mouse_x/30)+1] = '0';
                          moedas -= 10;
                          break;
                     case 1:
                          mat[(mouse_y/30)+1][(mouse_x/30)+1] = '1';
                          moedas -= 20;
                          break;
                     case 2:
                          mat[(mouse_y/30)+1][(mouse_x/30)+1] = '2';
                          moedas -= 50;
                          break;
             }
         }
         else{ /* Se, na matriz, na posição estiver uma arma */
             if( *troca_mouse==4 ){ /* e o mouse corresponder à 'excluir' */
                 switch( mat[(mouse_y/30)+1][(mouse_x/30)+1] ){ /* Dependendo da arma, aumenta o número de moedas */
                                  case '0':
                                       moedas += 5;
                                       break;
                                  case '1':
                                       moedas += 10;
                                       break;
                                  case '2':
                                       moedas += 25;
                                       break;
                                  default: /* Se não for uma arma */
                                          testa = -1;
                                          break;
                          }
                          if( testa==0 )
                              mat[(mouse_y/30)+1][(mouse_x/30)+1] = ' '; /* Apaga a arma do cenário */
             }
         }
         *troca_mouse = -1; /* Mouse padrão */
     }
}

/* Interações com o menu de opções */
void menu_opcoes( char **mat, int *troca_mouse ){
     
     int i, j;

         if( ( mouse_x >= 910) && (mouse_x <=990) && (mouse_y >=133) && (mouse_y<=250)){
             draw_sprite(buffer,menuarma1,910,260);
         }
         if( (mouse_x>=1000) && (mouse_x <=1080) && (mouse_y >=133) && (mouse_y<=250) ){
             draw_sprite(buffer,menuarma2,910,260);
            
         }
         if( (mouse_x>=1090) && (mouse_x <=1170) && (mouse_y >=133) && (mouse_y<=250)  ){
             draw_sprite(buffer,menuarma3,910,260);
            
         }
         
     if( mouse_b & 1 ){ /* Se o botão esquerdo for presseionado, muda o mouse dependendo do local */
         if( ( mouse_x >= 910) && (mouse_x <=990) && (mouse_y >=100) && (mouse_y<=200) && (moedas>=10) ){
              mouse = load_bitmap("imagens/mouse1.bmp",NULL);
             *troca_mouse = 0;
         }
         if( (mouse_x>=1000) && (mouse_x <=1080) && (mouse_y >=100) && (mouse_y<=200) && (moedas>=20) ){
              mouse = load_bitmap("imagens/mouse2.bmp",NULL);
             *troca_mouse = 1;
         }
         if( (mouse_x>=1090) && (mouse_x <=1170) && (mouse_y >=100) && (mouse_y<=200) && (moedas>=50) ){
             mouse = load_bitmap("imagens/mouse3.bmp",NULL);
             *troca_mouse = 2;
         }
         if( (mouse_x>=910) && (mouse_x <=990) && (mouse_y >=200) && (mouse_y<=300) ){
             mouse = load_bitmap("imagens/mouse4.bmp",NULL);
             *troca_mouse = 4;
         }
         if( (mouse_x>=1050) && (mouse_x <=1190) && (mouse_y >=562) && (mouse_y<=630) ){ /* Para volta para o menu principal */
             mouse = load_bitmap("imagens/mouse.bmp",NULL);
             voltar = 0; /* Volta para o menu principal */
             for( i=0; i<n_zumbi; i++ )
                  aux[i] = -1; /* Zera os contadores */
             free(mat); /* Libera a área alocada para matriz do cenário */
         }
         if( (mouse_x>=910) && (mouse_x<=1050) && (mouse_y >=562) && (mouse_y <=630)){ /* Se clicar em 'começar' */
              mouse = load_bitmap("imagens/mouse.bmp",NULL);
              for( i=0; i<n_zumbi; i++ )
                  if( aux[i]==-1 ){
                      zumbis[i].x = 0; /* Inicializa a posição x do zumbi */
                      zumbis[i].y = (rand()%18+1)*30; /* Inicializa a posição y do zumbi, aleatoriamente */
                      cont_y[i] = zumbis[i].y/30; /* Inicializa o contador y */
                      aux[i] = 0; /* Inicia o contador x */
                  }
             for ( i=0; i<MAX_ARMAS; i++ )
                 for( j=0; j<n_zumbi; j++ ){
                      tiros[i][j].ativo = 0; /* Desaiva os tiros */
                      tiros[i][j].arma = -1;
                 }
             }     
                 draw_sprite(buffer, mouse, mouse_x, mouse_y);   
    }    
}

/* Anda na vertical */
void desvia( int inicio, int *y, int x, int tam, int dir, int z ){
     /* inicio: representa a parte da imagem que será retirada */
     aux[z] = 1; /* Ativa o contador da vertical */
     
     if( dir==-1 ) /* Limpa a imagem horizontal do zumbi */
        inicio = 3;
        
     if( cont_y[z]<tam ){ /* Se o contador vertical for menor que o tamanho que o zumbi tem que andar */
         /* Desenha o zumbi, variando três imagens, de acordo com a direção */
         masked_blit( bmp, buffer, (cont_y[z]%3+inicio)*44, 50, x, *y+cont_y[z]*(30*dir), 44, 50 );
    }
    else{
        *y = *y+cont_y[z]*30*dir; /* A posição y do zumbi é acrescida */
        aux[z] = 0; /* Ativa o contador horizontal */
    }
}

/* Função utilizada para movimentar o zumbi horizontalmente */
int andar( int inicio, int passo, int x, int y, int z, int vida ){

    if( aux[z]==1 || vida <= 0 ){ /* Se o contador ativo for o cont_y e a vida do zumbi for zero */
        inicio = 3; /* imprime uma parte transparente da imagem */
    }
        
    masked_blit( bmp, buffer, (cont_x[z]%3+inicio)*44, 0, cont_x[z]*passo, y, 44, 50 );
        
    return (cont_x[z]*passo); /* Retorna a nova posição do zumbi */
}

/* A partir da matriz, gerada pela leitura do arquivo, imprime o cenário no buffer */
void desenha_fundo( char **mat, BITMAP *temp, int x, int y, char c ){
     
     int i, j, teste;
     
     for( i=0; i<y-1; i++ )
          for( j=0; j<=x; j++ )
               if( mat[j][i] == c )
                   draw_sprite(buffer, temp, i*30, j*30);
}

/* Lê o arquivo que representa o cenário a ser desenhado e coloca todos elementos desse arquivo numa matriz*/
char **copia_arquivo( char *nome, int *x, int *y ){
     
     int k;
     char **vet;
     char temp;
     FILE *arq = fopen( nome, "r");
     
     vet = (char **)malloc(50*sizeof(char *));

     for( k=0; k<50; k++ )
          vet[k] = (char *)malloc(50*sizeof(char));
        
     if( arq!=NULL ){
         while( !feof(arq) ){
                fscanf(arq, "%c", &temp);
                if( temp == '\n' ){
                    *x = *x+1;
                    *y = -1;
                }
                vet[*x][*y] = temp;
                *y = *y+1;
         }
         fclose(arq);
    }
    return vet; 
}

int creditos(){
    
    int credt=1;
    
    credito = load_bitmap("imagens/credito.bmp",NULL);
    
    while( credt==1 ){
           draw_sprite(buffer, credito, 0, 0);
           draw_sprite(buffer, mouse, mouse_x, mouse_y);
           blit(buffer, screen, 0, 0, 0, 0, Rx , Ry);
           
           if( (mouse_b & 1) && (mouse_x > 890) && (mouse_x < 1150) && (mouse_y < 612) && (mouse_y >550) )
                credt=0;
    }
    return credt;
}

int aju_da(){
    
    int ajud=1;
    
    ajuda = load_bitmap("imagens/ajuda.bmp",NULL);
    
    while( ajud==1 ){
           draw_sprite(buffer, ajuda, 0, 0);
           draw_sprite(buffer, mouse, mouse_x, mouse_y);
           blit(buffer, screen, 0, 0, 0, 0, Rx , Ry);
           
           if( (mouse_b & 1) && (mouse_x > 852) && (mouse_x < 1180) && (mouse_y < 602) && (mouse_y >545) )
               ajud=0;
  
    }
    return ajud;
}

int recor(){
    
    int recorde=1;
    int i = 0 ,j;
    int k = 0;
    int *vet;
 
    recordes = load_bitmap("imagens/recordes.bmp",NULL);
    
    while( recorde==1 ){
           draw_sprite(buffer, recordes, 0, 0);
           draw_sprite(buffer, mouse, mouse_x, mouse_y);
           
           FILE *arq = fopen("arquivos/score.txt", "r");
           
           vet = (int *)malloc(5*sizeof(int));
           
           if( arq!=NULL ){
               
               for( i=0; i<5; i++ ){ /* Copia as pontuações do arquivo para um vetor */
                    fscanf( arq, "%d", &vet[i] );      
               }
               fclose(arq); 
           }
           for( i=0; i<5; i++)
                textprintf_ex(buffer, font, 550, 200 +10*i, makecol(0,0,0), -1, "%do: %d  ",i+1, vet[i] );
           blit(buffer, screen, 0, 0, 0, 0, Rx , Ry);
           
           if( (mouse_b & 1) && (mouse_x > 852) && (mouse_x < 1180) && (mouse_y < 602) && (mouse_y >545) )
               recorde=0;
               
  }
  return recorde;
}

/* Calcula o tamanho do obstáculo em relação a posição do zumbi */
int calc_tamanho( char **mat, int coluna, int linha, int direcao ){
    
    int i, tam = 0;
    
    if( direcao==1 ){
        
        for( i=linha/30; i<Ry/30; i++ ){
             if(( mat[i][coluna/30+1] != '#' ) && ( mat[i][coluna/30+1] != '-' ) && (mat[i][coluna/30+1] != '!'))  
                 tam++;
             else
                 break;
        }    
    }
    else{
         for( i=linha/30; i<Ry/30; i-- ){
             if(( mat[i][coluna/30+1] != '#') && ( mat[i][coluna/30+1] != '-'  ) && (mat[i][coluna/30+1] != '!')) 
                 tam++;
             else
                 break;
        }     
    }
    
    return tam;
}

/* Altera a velocidade */
void define_veloc( int n ){
     
     veloc = 1000-n;
}

void inicializa(){
     
     int i;
     /* Carrega as imagens */
     buffer = create_bitmap(Rx , Ry);
     menu = load_bitmap("imagens/menujogo.bmp", NULL);
     chao = load_bitmap("imagens/chao1.bmp", NULL);
     bmp = load_bitmap("imagens/zombie.bmp", NULL);
     arma1 = load_bitmap("imagens/arma1.bmp", NULL);
     arma2 = load_bitmap("imagens/arma2.bmp", NULL);
     arma3 = load_bitmap("imagens/arma3.bmp", NULL);
     mouse = load_bitmap("imagens/mouse.bmp", NULL);
     base = load_bitmap("imagens/telhado4.bmp", NULL);
     fim = load_bitmap("imagens/telhado.bmp", NULL);
     bala = load_bitmap("imagens/tiro.bmp",NULL);
     menuarma1 = load_bitmap("imagens/menuarma1.bmp",NULL);
     menuarma2 = load_bitmap("imagens/menuarma2.bmp",NULL);
     menuarma3 = load_bitmap("imagens/menuarma3.bmp",NULL);
     perdeu = load_bitmap("imagens/gameover.bmp",NULL);
     arvore = load_bitmap("imagens/arvore.bmp",NULL);
     menu_principal = load_bitmap("imagens/menu_principal.bmp", NULL);
     pista = load_bitmap("imagens/pista.bmp",NULL);
    
     /* Bloqueia a área de memória ultilizada pelas variáveis */
     for( i=0; i<n_zumbi; i++ ){
          LOCK_VARIABLE(cont_x[i]);
          LOCK_VARIABLE(cont_y[i]);
     }
     LOCK_VARIABLE(n_zumbi);
     LOCK_FUNCTION(contador);
     install_int(contador, veloc);
}

/* Incremento dos contadores */
void contador(){
     
     int i;
     
     for( i=0; i<n_zumbi; i++ ){
          if( aux[i]==0 )
              cont_x[i]++;
          if( aux[i]==1 )
              cont_y[i]++;
          if( aux[i]==-1 )
              cont_x[i] = cont_y[i] = 0;             
     }
}
END_OF_FUNCTION(contador);

void deinit() {
	clear_keybuf();
}

void init(){
	int depth, res;
	allegro_init();
	depth = desktop_color_depth();
	if (depth == 0) depth = 32;
	set_color_depth(depth);
	res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, Rx,Ry, 0 , 0 );
	if (res != 0) {
		allegro_message(allegro_error);
		exit(-1);
	}

	install_timer();
	install_keyboard();
	install_mouse();
}
