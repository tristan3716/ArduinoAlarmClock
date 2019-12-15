enum Icon : byte {
    WaterDrop = 0,
    AngryFace = 1,
    SoSoFace = 2,
    SmileFace = 3
};

byte water_drop[] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000};

byte face_angry[] = {
    B01010,
    B01010,
    B01010,
    B00000,
    B00100,
    B01010,
    B10001,
    B00000};

byte face_soso[] = {
    B01010,
    B01010,
    B01010,
    B00000,
    B00000,
    B11111,
    B00000,
    B00000};

byte face_smile[] = {
    B01010,
    B01010,
    B01010,
    B00000,
    B10001,
    B10001,
    B01110,
    B00000};
