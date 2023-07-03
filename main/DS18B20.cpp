#ifndef __DS18B20__
#define __DS18B20__

#include "DS18B20.h"
#include "delay.h"

void DS18B20::capturaBit (int posicao,char v[], int valor)
{
	unsigned char pbyte = posicao / 8;
	unsigned char pbit  = posicao % 8;
	
	if (valor==1) v[pbyte] |= (1 << pbit);
	else v[pbyte] &= ~(1<< pbit);
}
void DS18B20::init (void)
{
	uint8_t serial[6],crc;
	delay_ms(500);

	printf("INIT\n");
	if (onewire->reset()==0) printf("Detectou escravo na linha\n");
	else printf("Nao detectou escravo\n");

	onewire->writeByte(READ_ROM);
	printf("Codigo da Familia: %d\n",onewire->readByte());
	for (uint8_t x=0;x<6;x++) serial[x] = onewire->readByte();

    printf("Numero de Serie  : %d %d %d %d %d %d\n",serial[0],serial[1],serial[2],serial[3],serial[4],serial[5]);

 	crc = onewire->readByte();
 	printf("CRC=             : %d\n",crc);

 	delay_ms(1000);


}
void DS18B20::init (char v[])
{

	delay_ms(500);

	printf("INIT\n");
	if (onewire->reset()==0) printf("Detectou escravo na linha\n");
	else printf("Nao detectou escravo\n");

	onewire->writeByte(READ_ROM);
	v[7] = onewire->readByte(); // family number
	printf("Codigo da Familia: %d\n",v[7]);
	
	for (uint8_t x=6;x>=1;x--) 
		v[x] = onewire->readByte(); //serial

    printf("Numero de Serie  : %d %d %d %d %d %d\n",v[1],v[2],v[3],v[4],v[5],v[6]);

 	v[0] = onewire->readByte();
 	printf("CRC=             : %d\n",v[0]);

 	delay_ms(1000);


}



// Usa o pino GPIO16 para fazer a comunicacao
DS18B20::DS18B20 (gpio_num_t pino)
{
	DEBUG("DS18B20:Construtor\n");
	onewire = new ONEWIRE(pino);
}

char DS18B20::CRC (char end[])
{
	// calcula o CRC e retorna o resultado
	return 0;
}
float DS18B20::readTargetTemp (char vetor_64bits[])
{
	float temperatura;

	uint8_t a,b,inteira;
	float frac;

	onewire->reset();
	onewire->writeByte(MATCH_ROM);
	for (int x=7;x>=0;x--)
	{
		onewire->writeByte(vetor_64bits[x]);
	}
	onewire->writeByte(INICIA_CONVERSAO_TEMP);
	delay_ms(1000);
	onewire->reset();
	onewire->writeByte(MATCH_ROM);
	for (int x=7;x>=0;x--)
	{
		onewire->writeByte(vetor_64bits[x]);
	}
	onewire->writeByte(READ_TEMP_MEMORY);
	a = onewire->readByte();
    b = onewire->readByte();
    inteira = (b<<4)|(a >> 4);


    	frac=((a & 1)*(0.0625))+ (((a>>1) & 1)*(0.125)) + (((a>>2) & 1)*(0.25)) + (((a>>3) & 1)*(0.5));
     
            temperatura = inteira + frac;
  return temperatura;     


	
}
float DS18B20::readTemp (void)
{
	float temperatura;

	uint8_t a,b,inteira;
	float frac;

	onewire->reset();
	onewire->writeByte(SKIP_ROM);
	onewire->writeByte(INICIA_CONVERSAO_TEMP);
	delay_ms(1000);
	onewire->reset();
	onewire->writeByte(SKIP_ROM);
	onewire->writeByte(READ_TEMP_MEMORY);
	a = onewire->readByte();
    b = onewire->readByte();
    inteira = (b<<4)|(a >> 4);


    	frac=((a & 1)*(0.0625))+ (((a>>1) & 1)*(0.125)) + (((a>>2) & 1)*(0.25)) + (((a>>3) & 1)*(0.5));
     
            temperatura = inteira + frac;
  return temperatura;     


	
}

/*void DS18B20::fazScan (void)
{
	char vetor[64];
	int8_t num_dvc = onewire->fazScan(vetor);
	char v[8]={0,0,0,0,0,0,0,0};
	uint8_t normal, complemento;
	
	onewire->reset();
	onewire->writeByte(SEARCH_ROM);
	
	for (int x=0;x<64;x++)
	{
		normal = onewire->readBit();
		complemento = onewire->readBit();
		if (normal==0 && complemento==0)
		{	
			// bits conflitantes na posicao


			onewire->escreve_bit(0);
			capturaBit(x,v,0);
		}
		if (normal==0 && complemento==1)
		{
			// o bit é 0

			capturaBit(x,v,0);
			onewire->escreve_bit(0);
		}
		if (normal==1 && complemento==0)
		{
			// o bit é 1

			capturaBit(x,v,1);
			onewire->escreve_bit(1);
		}
		if (normal==1 && complemento==1)
		{
			// nao existem escravos no barramento
			printf("Nao existem escravos no barramento\n");
			return;
		}
		
	}
	printf("Codigo da Familia: %d\n",v[0]);
    	printf("Numero de Serie  : %d %d %d %d %d %d\n",v[6],v[5],v[4],v[3],v[2],v[1]);
 	printf("CRC=             : %d\n",v[7]);
 	
 	printf("Endereco completo: %d %d %d %d %d %d %d %d\n",v[7],v[6],v[5],v[4],v[3],v[2],v[1],v[0]);

}*/

uint8_t DS18B20::fazScan(char* vetor) {
    DEBUG("DS18B20:fazScan\n");

    if (onewire->reset()) return 0;

    // bits de confirmacao do endereco one wire
    int8_t id_bit = 0;
    int8_t cmp_id_bit = 0;

    int8_t id_bit_number = 0; // bit atual sendo processado
    int8_t last_discrepancy[64] = {0}; // posicoes das discrepancias encontradas (pilha)
    int8_t last_discrepancy_bit_number = 64; // posicao da ultima discrepancia

    uint8_t qnt_sensores = 0; // quantidade de sensores encontrados
    uint8_t last_dvc[64] = {0}; // endereco do ultimo dispositivo

	

    // laco de dispositivos
    do
    {
        id_bit_number = 0;
		int aux = 1;
        onewire->writeByte(SEARCH_ROM);

        // laco do endereco atual
        do {
            id_bit = onewire->readBit();
            cmp_id_bit = onewire->readBit();
			printf("id_bit: %d cmp_id_bit: %d\n",id_bit,cmp_id_bit);

            if ((id_bit == 1) && (cmp_id_bit == 1)) return qnt_sensores;

            if(qnt_sensores > 0 && aux) {
				printf("Outro sensor encontrado\n");
                // laco para achar a ultima discrepancia salva
                for(last_discrepancy_bit_number = 63; last_discrepancy_bit_number >= 0; last_discrepancy_bit_number--)
                {
                    if(last_discrepancy[last_discrepancy_bit_number]) break;
                }

                // laco para chegar na ultima discrepancia
                do
                {
					onewire->escreve_bit(last_dvc[id_bit_number]);
					id_bit = onewire->readBit();
            		cmp_id_bit = onewire->readBit();
                    id_bit_number++;
					printf("id_bit: %d cmp_id_bit: %d\n",id_bit,cmp_id_bit);
                } while (id_bit_number < last_discrepancy_bit_number);

                // desabilitar a last discrepancy se mais de um dvc
                last_discrepancy[last_discrepancy_bit_number] = 0;
                
                id_bit = 1;
				aux = 0;
            }


            if ((id_bit == 0) && (cmp_id_bit == 0)) { 
                last_discrepancy[id_bit_number] = 1;

				printf("Discrepancia encontrada: %d\n", id_bit_number);
            }
            
            last_dvc[id_bit_number] = id_bit;
            id_bit_number++;
			printf("id_bit: %d id_bit_number: %d cmp_id_bit: %d\n", id_bit, id_bit_number, cmp_id_bit);
            onewire->escreve_bit(id_bit);

            

        } while (id_bit_number < 64);
        
		onewire->reset();

		// salvara last dvc to vetor
		uint8_t s = 0;
        for (int i = 63; i >= 0; i--) {
			uint8_t aux2 = 1;
			for (int j = 0; j < i % 8; j++) aux2 *= 2; //calcula potencia do 2

			s += (last_dvc[i] * aux2); //multiplica potencia do 2 pelo bit

			printf("%d", last_dvc[i]);
			if(i % 8 == 0) {	
				vetor[qnt_sensores*8 + ((63-i) / 8)] = s;
				printf(" - s: %d\n", s);
				s = 0;
			}
        }

		qnt_sensores++;
    } while (last_discrepancy[0] != 0 || last_discrepancy_bit_number > 0);
    
    return qnt_sensores;    
}

#endif
