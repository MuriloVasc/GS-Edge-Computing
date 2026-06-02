#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int NUM_LEDS     = 15;
const int LED_VERDE[5]    = {2, 3, 4, 5, 6};
const int LED_AMARELO[5]  = {7, 8, 9, 10, 11};
const int LED_VERMELHO[5] = {12, 13, A1, A2, A3};
const int LED_PINS[15]    = {2,3,4,5,6,7,8,9,10,11,12,13,A1,A2,A3};

const int POT_PIN    = A0;
const int BTN_PIN    = 0;
const int BUZZER_PIN = 1;

const int QBER_LIMITE = 11;

int totalSessoes  = 0;
int totalSucesso  = 0;

void aguardarBotao();
void faseEmissao();
void faseMedicao();
bool faseVerificacao();
void animacaoSucesso();
void animacaoFalha();
void exibirEspera();
void exibirFase(const char* linha1, const char* linha2);
void beepSucesso();
void beepAlerta();
void beepFalha();
void apagarTodos();
int lerQBER();

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" QKD SATELLITE  ");
  lcd.setCursor(0, 1);
  lcd.print("  FIAP  GS 2025 ");
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(60);
  }
  delay(600);
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(LED_PINS[i], LOW);
    delay(60);
  }
  delay(500);
  lcd.clear();
}

void loop() {
  exibirEspera();
  aguardarBotao();

  totalSessoes++;

  faseEmissao();
  faseMedicao();
  bool sucesso = faseVerificacao();

  if (sucesso) {
    totalSucesso++;
    animacaoSucesso();
  } else {
    animacaoFalha();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sessao ");
  lcd.print(totalSessoes);
  lcd.print(sucesso ? " OK " : " FAIL");
  lcd.setCursor(0, 1);
  int taxa = (totalSessoes > 0) ? (totalSucesso * 100 / totalSessoes) : 0;
  lcd.print("Taxa OK: ");
  lcd.print(taxa);
  lcd.print("%   ");

  delay(3000);
  apagarTodos();
  lcd.clear();
}

void aguardarBotao() {
  while (digitalRead(BTN_PIN) == HIGH) {
    delay(50);
  }
  delay(200);
}

void faseEmissao() {
  exibirFase("FASE 1: EMISSAO ", "Fotons enviados ");
  beepAlerta();
  delay(400);
  for (int i = 0; i < 5; i++) {
    lcd.setCursor(0, 1);
    lcd.print("Foton ");
    lcd.print(i + 1);
    lcd.print("/5      ");
    digitalWrite(LED_VERDE[i], HIGH);
    delay(250);
  }
  delay(500);
}

void faseMedicao() {
  exibirFase("FASE 2: MEDICAO ", "Medindo estados ");
  delay(400);
  for (int i = 0; i < 5; i++) {
    lcd.setCursor(0, 1);
    lcd.print("Estado ");
    lcd.print(i + 1);
    lcd.print("/5     ");
    for (int p = 0; p < 2; p++) {
      digitalWrite(LED_AMARELO[i], HIGH);
      tone(BUZZER_PIN, 1200, 40);
      delay(150);
      digitalWrite(LED_AMARELO[i], LOW);
      delay(100);
    }
    digitalWrite(LED_AMARELO[i], HIGH);
    delay(150);
  }
  delay(400);
}

bool faseVerificacao() {
  exibirFase("FASE 3: VERIF.  ", "Calc. QBER...   ");
  delay(600);
  for (int i = 0; i < 5; i++) {
    int qber = lerQBER();
    lcd.setCursor(0, 1);
    lcd.print("QBER: ");
    lcd.print(qber);
    lcd.print("%  ");
    digitalWrite(LED_VERMELHO[i], HIGH);
    delay(300);
    if (i >= 2 && qber >= QBER_LIMITE) {
      for (int r = i + 1; r < 5; r++) {
        for (int p = 0; p < 3; p++) {
          digitalWrite(LED_VERMELHO[r], HIGH);
          delay(120);
          digitalWrite(LED_VERMELHO[r], LOW);
          delay(120);
        }
        digitalWrite(LED_VERMELHO[r], HIGH);
      }
      lcd.setCursor(0, 0);
      lcd.print("INTERCEPTACAO!! ");
      lcd.setCursor(0, 1);
      lcd.print("QBER: ");
      lcd.print(qber);
      lcd.print("% ALTO");
      return false;
    }
  }
  int qberFinal = lerQBER();
  if (qberFinal >= QBER_LIMITE) {
    lcd.setCursor(0, 0);
    lcd.print("INTERCEPTACAO!! ");
    lcd.setCursor(0, 1);
    lcd.print("QBER: ");
    lcd.print(qberFinal);
    lcd.print("% ALTO");
    return false;
  }
  lcd.setCursor(0, 0);
  lcd.print("CHAVE  ACEITA!  ");
  lcd.setCursor(0, 1);
  lcd.print("QBER: ");
  lcd.print(qberFinal);
  lcd.print("% OK  ");
  return true;
}

void animacaoSucesso() {
  beepSucesso();
  for (int rep = 0; rep < 2; rep++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], HIGH);
      delay(40);
    }
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      digitalWrite(LED_PINS[i], LOW);
      delay(40);
    }
  }
  for (int v = 0; v < 3; v++) {
    for (int i = 0; i < NUM_LEDS; i++) digitalWrite(LED_PINS[i], HIGH);
    delay(180);
    for (int i = 0; i < NUM_LEDS; i++) digitalWrite(LED_PINS[i], LOW);
    delay(180);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" CHAVE SEGURA!  ");
  lcd.setCursor(0, 1);
  lcd.print(" QKD CONCLUIDO  ");
  delay(2000);
}

void animacaoFalha() {
  beepFalha();
  for (int v = 0; v < 5; v++) {
    for (int i = 0; i < 5; i++) digitalWrite(LED_VERMELHO[i], HIGH);
    delay(120);
    for (int i = 0; i < 5; i++) digitalWrite(LED_VERMELHO[i], LOW);
    delay(120);
  }
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_VERDE[i], LOW);
    digitalWrite(LED_AMARELO[i], LOW);
    digitalWrite(LED_VERMELHO[i], HIGH);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHAVE DESCART.  ");
  lcd.setCursor(0, 1);
  lcd.print("Reiniciando...  ");
  delay(2000);
}

void exibirEspera() {
  lcd.setCursor(0, 0);
  lcd.print("Pressione botao ");
  lcd.setCursor(0, 1);
  lcd.print("Sessoes: ");
  lcd.print(totalSessoes);
  lcd.print("        ");
}

void exibirFase(const char* linha1, const char* linha2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
}

int lerQBER() {
  int val = analogRead(POT_PIN);
  return map(val, 0, 1023, 0, 25);
}

void apagarTodos() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}

void beepSucesso() {
  tone(BUZZER_PIN, 880, 100); delay(120);
  tone(BUZZER_PIN, 1100, 100); delay(120);
  tone(BUZZER_PIN, 1320, 200); delay(250);
  noTone(BUZZER_PIN);
}

void beepAlerta() {
  tone(BUZZER_PIN, 1000, 80);
  delay(100);
  noTone(BUZZER_PIN);
}

void beepFalha() {
  tone(BUZZER_PIN, 400, 200); delay(220);
  tone(BUZZER_PIN, 300, 400); delay(450);
  noTone(BUZZER_PIN);
}