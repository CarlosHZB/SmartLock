#include <Keypad.h> // Biblioteca do codigo

const byte LINHAS = 4; // Linhas do teclado
const byte COLUNAS = 4; // Colunas do teclado

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado)
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

 byte PINOS_LINHAS[LINHAS] = {D4, D5, D6, D7}; // Pinos de conexao com as linhas do teclado
 byte PINOS_COLUNAS[COLUNAS] = {D0, D1, D2, D3}; // Pinos de conexao com as colunas do teclado

Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS); // Inicia teclado

void setup() {
  Serial.begin(9600); // Inicia porta serial
}

String senha_certa = "1234";
String senha_digitada = "";

void loop() {

  char leitura_teclas = teclado_personalizado.getKey(); 
  
  if(leitura_teclas) { 
    Serial.println(leitura_teclas); 
     switch(leitura_teclas)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
            senha_digitada+=leitura_teclas; 
            Serial.println(senha_digitada);
            break;  
      case '*':
              //limpa a variável que guarda a senha 
              if(senha_digitada != "")
              {
               senha_digitada = "";
               Serial.println("Senha digitada apagada"); 
              }
              break;
      case '#':
              //verifica se a senha tá certa  
              if(senha_digitada != ""){
                if(senha_certa==senha_digitada)
                {
                  Serial.println("Senha Correta!");
                  digitalWrite(HIGH, D0);
                  delay(5000);
                  digitalWrite(LOW, D0);
                }
                //caso senha esteja errada
                else{
                  Serial.println("Senha Incorreta!");
                }
              }                         
              senha_digitada = "";
              break;
      default: break;
      
    }

  }
  
}
