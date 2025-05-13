//  while(1) {
//  	    // 1. Đọc trạng thái hiện tại của nút nhấn
//  	    GPIO_PinState current_button_state_gpio = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
//  // 1 = Nhả (HIGH), 0 = Nhấn (LOW)
//
//  	    // 2. Kiểm tra sự thay đổi trạng thái từ Nhả sang Nhấn
//  	    while (current_button_state_gpio == 1)
//  	    {
//  	    	HAL_Delay(50);
//  	      // 3. Chống rung (debounce)
//  	      if (current_button_state_gpio == 1)
//  	      {
//
//  	        // Đã đủ thời gian chống rung -> xác nhận nhấn nút
//
//  	        // 4. Đảo trạng thái LED
//  	        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
//
//  	      }
//  	      current_button_state_gpio = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
//  	    }
//          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
//
//  	  }
