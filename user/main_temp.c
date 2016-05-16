
	while(1)
	{
	//***** Voltage convert *********************************************/
		GlobalVariable.Voltage = (GlobalVariable.Voltage*4 + ((ADC_GetConversionValue (ADC1)*3690)>>12)*6)/10; 
		//the number 3690 is calcualted by me, according to real results
	
	//***** read water temperature **************************************/
		//ds_convert();

	//***** use ADC1 channel_16 + DMA1 to read temperature sensor********/
		sensor = 0; 		
		for(i=0;i<50;i++){
			sensor += ADCConvert[i][0];
		}
		tempf = sensor/50.0f;
		tempf = (float)(tempf*3.3f/4096.0f);
		GlobalVariable.tempInternal = (1.43f-tempf)/0.0043f + 25.0f + 40;
		GlobalVariable.tempInternalByte = (u8)((int)GlobalVariable.tempInternal);
		//delay_ms(1);	

	//***** CAN receive navigation data from navigation board*************/
		if(CAN_MessagePending(CAN1, 0))
		{						  
			CAN_Receive	(CAN1, 0, &RxMessage);
			if(RxMessage.Data[0] == 0x11) 
			{ 	
				switch(RxMessage.Data[1]){
					case 0x00:
						for(i=0; i<4; i++){
							GlobalVariable.GPS.Latitude[i]=RxMessage.Data[2+i];							
						}
						break;
					case 0x01:
						for(i=0; i<4; i++){
							GlobalVariable.GPS.Latitude[4+i]=RxMessage.Data[2+i];							
						}
						// convert latitude to double, for debug
						for(i=0;i<8;i++){
							LatBytes.bArray[i] = GlobalVariable.GPS.Latitude[i];
						}
						LatDouble = BytesToDouble(LatBytes);
						break;
 					case 0x02:
						for(i=0; i<4; i++){
							GlobalVariable.GPS.Longitude[i]=RxMessage.Data[2+i];							
						}
						break;
					case 0x03:
						for(i=0; i<4; i++){
							GlobalVariable.GPS.Longitude[4+i]=RxMessage.Data[2+i];							
						}
						// convert latitude to double, for debug
						for(i=0;i<8;i++){
							LonBytes.bArray[i] = GlobalVariable.GPS.Longitude[i];
						}
						LonDouble = BytesToDouble(LonBytes);
						break;
					case 0x04:
						GlobalVariable.Compass.heading[0]=RxMessage.Data[2];	
						GlobalVariable.Compass.heading[1]=RxMessage.Data[3];
						break;
					case 0x05:
						GlobalVariable.Compass.pitch[0]=RxMessage.Data[2];	
						GlobalVariable.Compass.pitch[1]=RxMessage.Data[3];
						break;
					case 0x06:
						GlobalVariable.Compass.roll[0]=RxMessage.Data[2];	
						GlobalVariable.Compass.roll[1]=RxMessage.Data[3];
						break;
					case 0x07:
						GlobalVariable.GPS.Speed[0]=RxMessage.Data[2];	
						GlobalVariable.GPS.Speed[1]=RxMessage.Data[3];	 
						break;
					case 0x08:
						GlobalVariable.GPS.Time[0]=RxMessage.Data[2];	
						GlobalVariable.GPS.Time[1]=RxMessage.Data[3];
						GlobalVariable.GPS.Time[2]=RxMessage.Data[4];
						break;
					case 0x09:
						GlobalVariable.GPS.Date[0]=RxMessage.Data[2];	
						GlobalVariable.GPS.Date[1]=RxMessage.Data[3];
						GlobalVariable.GPS.Date[2]=RxMessage.Data[4];
						break;
				}// end of case
			}// end of data from 0x11
		}//end of CAN receive data
		
	//***** Control rudder, propeller, send command to valve***********/					  			
		if(RxEndFlag == TRUE)
		{	      
			for (command_index = 0; command_index < CommandLength-2; command_index++){
				command_checksum ^= Command[command_index];
			}
			if(command_checksum==Command[CommandLength-1]){
				// command index 0:rudder 1:propeller 2:alarm lamp 3:alarm clock 4:cam power 5:valve 6:pump 7:收放杆			
				// 与外挂桨板通信
				TxMessage2.StdId = 0x12;
				TxMessage2.DLC = 4;
				TxMessage2.Data[0] = 0x10;
				TxMessage2.Data[1] = Command[1]; 	//rudder
				TxMessage2.Data[2] = Command[3];  	//propeller
				TxMessage2.Data[3] = TxMessage2.Data[2]^TxMessage2.Data[1];	
				CAN_TX (&TxMessage2);
				
				for (command_index = 4; command_index < CommandLength-3; command_index+=2)
				{
					switch(Command[command_index])
					{
						case 2:
							switch(Command[command_index+1]){
								case 33:
									ALARM_RESET;
									break;
								case 17:
									ALARM_SET;
									break;
								case 65:
									break;
								case 129:
									break;
							}							
							break;
						case 3:
							switch(Command[command_index+1]){
								case 33:
									ALERT_LED_RESET;
									break;
								case 17:
									ALERT_LED_SET;
									break;
							}
							break;
						case 4:
							switch(Command[command_index+1]){
								case 33:
									CAM_PWR_RESET;
									break;
								case 17:
									CAM_PWR_SET;
									break;
							}
							break;
						case 5:
						/*
							VALVE1_CLOSE;
							VALVE2_CLOSE;
							VALVE3_CLOSE;
							VALVE4_CLOSE;
							switch(Command[command_index+1]){
								case 101:
									VALVE1_OPEN;
									break;
								case 102:
									VALVE2_OPEN;
									break;
								case 103:
									VALVE3_OPEN;
									break;
								case 104:
									VALVE4_OPEN;
									break;
								case 105:
									VALVE1_OPEN;
									VALVE2_OPEN;
									VALVE3_OPEN;
									VALVE4_OPEN;
									break;
							}	 */
							break;
						case 6:
							switch(Command[command_index+1]){
								case 33:
									PUMP_STOP;
									break;
								case 17:
									PUMP_START;
									break;
							}
							break;
						case 7:
							switch(Command[command_index+1]){
								case 33:
									ROD_DOWN_RESET;
									ROD_UP_RESET;
									break;
								case 17:
									ROD_UP_SET;
									ROD_DOWN_RESET;
									rod_down = 0;
									break;
								case 65:
									ROD_DOWN_SET;
									ROD_UP_RESET;
									rod_down = 1;
									break;
							}
							break;
						case 8:
							break;
						case 9:
							break;
					}
				} 					
			}			
			RxEndFlag = FALSE;
		}


	//***** send navigation data via USART to ARM9 ************************/
		//Generate the header
		PutUART(0);
		PutUART('B');
		PutUART(1);
		PutUART(124);

		//Latitude 01
		PutUART(1);	   
		for(i = 0; i<8; i++)
		{
			PutUART (GlobalVariable.GPS.Latitude[i]);
		}						
		PutUART(124);

	   	//Longitude 02
		PutUART(2);
		for(i = 0; i<8; i++)
		{
			PutUART (GlobalVariable.GPS.Longitude[i]);
		}
		PutUART(124);

	   	//Speed 03
		PutUART(3);	
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.GPS.Speed[i]);
		}	
		PutUART(124);

		//time 04
		PutUART(4);	
		for(i = 0; i<3; i++)
		{
			PutUART (GlobalVariable.GPS.Time[i]);
		}	
		PutUART(124);

		//date 05
		PutUART(5);	
		for(i = 0; i<3; i++)
		{
			PutUART (GlobalVariable.GPS.Date[i]);
		}	
		PutUART(124);
			
		//Power Life 06
		PutUART(6);
		if (GlobalVariable.Voltage >= 1680){
			buf = 100;
		}else if (GlobalVariable.Voltage >= 1632){
			buf = 90;
		}else if (GlobalVariable.Voltage >= 1600){
			buf = 80;
		}else if (GlobalVariable.Voltage >= 1572){
			buf = 70;
		}else if (GlobalVariable.Voltage >= 1548){
			buf = 60;
		}else if (GlobalVariable.Voltage >= 1528){
			buf = 50;
		}else if (GlobalVariable.Voltage >= 1516){
			buf = 40;
		}else if (GlobalVariable.Voltage >= 1508){
			buf = 30;
		}else if (GlobalVariable.Voltage >= 1492){
		 	buf = 20;
		}else if (GlobalVariable.Voltage >= 1480){
			buf = 15;
		}else if (GlobalVariable.Voltage >= 1472){
			buf = 10;
		}else if (GlobalVariable.Voltage >= 1400){
			buf = 5;
		}else{
			buf = 0;
		}
		PutUART (buf);	
		PutUART(124);

		//Compass pitch 10
		PutUART(10);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.pitch[i]);
		}	
		PutUART(124);

		//Compass roll 11
		PutUART(11);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.roll[i]);
		}	
		PutUART(124);

		//Compass yaw 12
		PutUART(12);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.heading[i]);
		}	
		PutUART(124);

		// internal temperature 19
		PutUART(19);
		PutUART (GlobalVariable.tempInternalByte);
		PutUART(124);

		// water temperature 81
		PutUART(81);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.tempWaterBytes[i]);
		}	
		PutUART(124);

		PutString ("\r\n");
	
		delay_us(1000000);

	//***** Receive control data via USART to ARM9 ************************/


	}	//end of while 1
