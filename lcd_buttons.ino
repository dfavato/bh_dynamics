void lcd_buttons_loop() {
  while(1) {
    int botao = analogRead(0);
    char text[50];
    sprintf(text, "Valor: %d", botao);
    interface.set_lcd_text("Botoes", text);
    interface.refresh_lcd(); 
  }
}

