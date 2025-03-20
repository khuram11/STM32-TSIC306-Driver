
#define TMP_PIN_HIGH 	(GPIOB->IDR & 1 )
#define TMP_PIN_LOW   	(!TMP_PIN_HIGH)

void WAIT_FOR_TMP_LOW(const uint32_t Time)
{
	while (TMP_PIN_HIGH && ((HAL_GetTick() - Time) < 2000));

}
void WAIT_FOR_TMP_HIGH(const uint32_t Time)
{
	while (TMP_PIN_HIGH && ((HAL_GetTick() - Time) < 2000));
}
void WAIT_FOR_TSTROBE(const uint16_t CNT, uint32_t Time)
{
	TIM1->CNT = 0;
	while (TIM1->CNT < CNT)
	{
		if ((HAL_GetTick() - Time) > 2000)
			{
			return ;
			}
	}
}

int8_t read_tmp_Digital (void)
{

	const uint32_t st_time = HAL_GetTick();
	WAIT_FOR_TMP_LOW(st_time);					// could be in middle of a packet, read the next packet
	HAL_Delay(20); 								      //let the current packet end
	HAL_TIM_Base_Start(&htim1); 				// Timer 1 with 1 uSec Ticks
	WAIT_FOR_TMP_LOW(st_time);
	uint32_t Tstrobe1 = TIM1->CNT;
	WAIT_FOR_TMP_HIGH(st_time);
	uint16_t Tstrobe2 = TIM1->CNT; 				// to get the strobe time
	const uint16_t Tstrobe = Tstrobe2 - Tstrobe1;
	//wait for next falling edge to start reading data

	uint8_t TMP_H = 0;
	uint8_t TMP_L = 0;
	for(int i = 7 ; i >= 0 ; i-- )
	{
		WAIT_FOR_TMP_LOW(st_time);
		WAIT_FOR_TSTROBE(Tstrobe, st_time);
		TMP_H |= ( GPIOB->IDR & 0b1 ) << i;
	}
	for(int i = 7 ; i >= 0 ; i-- )
	{
		WAIT_FOR_TMP_LOW(st_time);
		WAIT_FOR_TSTROBE(Tstrobe, st_time);
		TMP_L |= ( GPIOB->IDR & 0b1 ) << i;
	}
	uint16_t TMP = ((uint16_t)TMP_H << 8) | TMP_L;

	float temp = ( TMP / 2047.0f ) * (200.0f) - 50.0f;
	return (int8_t)temp;

}
