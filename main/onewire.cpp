
#include "onewire.h"
#include "digital.h"
#include "delay.h"

ONEWIRE::ONEWIRE(gpio_num_t p)
{
	DEBUG("ONEWIRE:construtor\n");
	PIN_DADOS = p;
}

void ONEWIRE::low(void)
{
	digital.pinMode(PIN_DADOS, OUTPUT);
     digital.digitalWrite(PIN_DADOS,LOW);
}
void ONEWIRE::high(void)
{
     digital.pinMode(PIN_DADOS, INPUT);
}
uint8_t ONEWIRE::reset (void)
{
	uint8_t resposta;
	low();
	delay_us(500);
	high();
	delay_us(60);
	resposta=digital.digitalRead(PIN_DADOS);
  	delay_us(600);
  	return resposta;
}


uint8_t ONEWIRE::readBit(void)
{
    uint8_t c;
     low();
      delay_us(5);
      high();
      c = digital.digitalRead(PIN_DADOS);
      delay_us(70);
      return c;
}
uint8_t ONEWIRE::readByte(void)
{
    uint8_t valor,x,c;
    
    valor  = 0;
   for (x=0;x<8;x++)
   {
      low();
      delay_us(5);
      high();
      c = digital.digitalRead(PIN_DADOS);
      valor = (valor  ) | (c<<x);
      delay_us(70);
   }
   return valor;

}
void ONEWIRE::escreve_bit(uint8_t meu_bit)
{
  if (meu_bit)
  {
      low();
      delay_us(5);
      high();
      delay_us(60);
  }
  else
  {
      low();
      delay_us(80);
      high(); 
  }
  delay_us(5);
}
void ONEWIRE::writeByte(uint8_t v)
{
	DEBUG("ONEWIRE:writeByte\n");

	uint8_t x;
  	for (x=0;x<8;x++)
  	{
    		escreve_bit((v >> x) & 1); 
  	}

	
}

uint8_t ONEWIRE::fazScan(char* vetor) {
    DEBUG("ONEWIRE:fazScan\n");

    if (reset()) return 0;

    // bits de confirmacao do endereco one wire
    int8_t id_bit = 0;
    int8_t cmp_id_bit = 0;

    int8_t id_bit_number = 0; // bit atual sendo processado
    int8_t last_discrepancy[64] = {0}; // posicoes das discrepancias encontradas (pilha)
    int8_t last_discrepancy_bit_number = 64; // posicao da ultima discrepancia

    int8_t qnt_sensores = 0; // quantidade de sensores encontrados
    int8_t last_dvc[64] = {0}; // endereco do ultimo dispositivo


    // laco de dispositivos
    do
    {
        id_bit_number = 0;
        writeByte(SEARCH_ROM);

        // laco do endereco atual
        do {
            id_bit = readBit();
            cmp_id_bit = readBit();

            if ((id_bit == 1) && (cmp_id_bit == 1)) return 0;

            if(qnt_sensores > 0) {

                // laco para achar a ultima discrepancia salva
                for(last_discrepancy_bit_number = 64; last_discrepancy_bit_number > 0; last_discrepancy_bit_number--)
                {
                    if(last_discrepancy[last_discrepancy_bit_number]) break;
                }

                // laco para chegar na ultima discrepancia
                do
                {
                    escreve_bit(last_dvc[id_bit_number]);
                    id_bit_number++;
                } while (id_bit_number < last_discrepancy_bit_number);

                // desabilitar a last discrepancy se mais de um dvc
                last_discrepancy[last_discrepancy_bit_number] = 0;
                // 
                id_bit = 1;
            }


            if ((id_bit == 0) && (cmp_id_bit == 0)) { 
                last_discrepancy[id_bit_number] = 1;
            }
            
            last_dvc[id_bit_number] = id_bit;
            id_bit_number++;
            escreve_bit(id_bit);

            

        } while (id_bit_number < 64);
        // salvara last dvc to vetor
        for (int i = 0; i < 64; i++) {
            printf("BIT[%d] = %d", i, last_dvc[i]);
        }
        
        
        qnt_sensores++;
    } while (last_discrepancy[0] != 0 || last_discrepancy_bit_number > 0);
    
    

    return qnt_sensores;
    
    



    
    return 0;
}