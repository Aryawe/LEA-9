//-------------------------//
//------[ VARIABLES ]------//
//-------------------------//
#define N  (3)
#define INDEX(x, y) ((x*N)+y)

boolean invert_matrix(byte&, byte&) ;

void setup()
{
  Serial.begin(9600) ;

  // Random start point.
  randomSeed(analogRead(0));
  
  Serial.println("> Ready.") ;
}

void loop()
{
  byte A[N][N] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
  } ;
  byte B[N][N] {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
  } ;

  // Generate random key
  do
  {
    for (int line = 0; line < N; line++)
    {
       for (int row = 0; row < N; row++)
       {
          A[line][row] = random(256) ;
       }
    }
  } while (modInverse((A[0][0] * ((A[1][1] * A[2][2]) - (A[1][2] * A[2][1]))) -
                      (A[0][1] * ((A[1][0] * A[2][2]) - (A[1][2] * A[2][0]))) +
                      (A[0][2] * ((A[1][0] * A[2][1]) - (A[1][1] * A[2][0]))), 
          256) == 0) ;
  
  // Compute private key
  boolean success = invert_matrix(*A, *B) ;
  if (success)
  {
    Serial.println("> Generated public key :") ;
    printKey(*A) ;
    // Display key
    Serial.println("> Associated private key :") ;
    printKey(*B) ;
    Serial.println("") ;
  } else
  {
    // determinant is not coprime with 256 !
    //Serial.println("> Error : Not inveretible.") ;
  }

  // Not overloading
  delay(100) ;
}


//--------------------------//
//----[ MATH FUNCTIONS ]----//
//--------------------------//
boolean invert_matrix(byte* pA, byte* pB)
{
  // Compute modular inverse of the determinant.
  int alpha = modInverse((pA[INDEX(0, 0)] * ((pA[INDEX(1, 1)] * pA[INDEX(2, 2)]) - (pA[INDEX(1, 2)] * pA[INDEX(2, 1)]))) -
                         (pA[INDEX(0, 1)] * ((pA[INDEX(1, 0)] * pA[INDEX(2, 2)]) - (pA[INDEX(1, 2)] * pA[INDEX(2, 0)]))) +
                         (pA[INDEX(0, 2)] * ((pA[INDEX(1, 0)] * pA[INDEX(2, 1)]) - (pA[INDEX(1, 1)] * pA[INDEX(2, 0)])))
              , 256) ;

  // Generate private key
  if (alpha != 0) 
  {
    pB[INDEX(0, 0)] = (byte)( (int)( ((pA[INDEX(1, 1)] * pA[INDEX(2, 2)]) - (pA[INDEX(1, 2)] * pA[INDEX(2, 1)])) * alpha) % 256 ) ;
    pB[INDEX(0, 1)] = (byte)( (int)( ((pA[INDEX(0, 2)] * pA[INDEX(2, 1)]) - (pA[INDEX(0, 1)] * pA[INDEX(2, 2)])) * alpha) % 256 ) ;
    pB[INDEX(0, 2)] = (byte)( (int)( ((pA[INDEX(0, 1)] * pA[INDEX(1, 2)]) - (pA[INDEX(0, 2)] * pA[INDEX(1, 1)])) * alpha) % 256 ) ;
    
    pB[INDEX(1, 0)] = (byte)( (int)( ((pA[INDEX(1, 2)] * pA[INDEX(2, 0)]) - (pA[INDEX(1, 0)] * pA[INDEX(2, 2)])) * alpha) % 256 ) ;
    pB[INDEX(1, 1)] = (byte)( (int)( ((pA[INDEX(0, 0)] * pA[INDEX(2, 2)]) - (pA[INDEX(0, 2)] * pA[INDEX(2, 0)])) * alpha) % 256 ) ;
    pB[INDEX(1, 2)] = (byte)( (int)( ((pA[INDEX(0, 2)] * pA[INDEX(1, 0)]) - (pA[INDEX(0, 0)] * pA[INDEX(1, 2)])) * alpha) % 256 ) ;
    
    pB[INDEX(2, 0)] = (byte)( (int)( ((pA[INDEX(1, 0)] * pA[INDEX(2, 1)]) - (pA[INDEX(1, 1)] * pA[INDEX(2, 0)])) * alpha) % 256 ) ;
    pB[INDEX(2, 1)] = (byte)( (int)( ((pA[INDEX(0, 1)] * pA[INDEX(2, 0)]) - (pA[INDEX(0, 0)] * pA[INDEX(2, 1)])) * alpha) % 256 ) ;
    pB[INDEX(2, 2)] = (byte)( (int)( ((pA[INDEX(0, 0)] * pA[INDEX(1, 1)]) - (pA[INDEX(0, 1)] * pA[INDEX(1, 0)])) * alpha) % 256 ) ;

    // Check key...    
    if (checkKey(pA, pB))
    {
      return true ;
    } else
    {
      return false ;
    }
    
  }
  else
  {
    return false ;
  }
}

int modInverse(int t, int m)
{
    t = t%m;
    for (int x=1; x<m; x++)
       if ((t*x) % m == 1)
          return x ;
}


boolean checkKey(byte* vA, byte* vB)
{
  boolean authentic = true ;
  for (int k = 0; k < 3; k++)
  {
    byte v[N] = {(byte)random(256), (byte)random(256), (byte)random(256)}; // Raw data
    byte w[N] = {0, 0, 0}; // Encrypted data
    byte q[N] = {0, 0, 0}; // Decrypted data
    
    w[0] = ((vA[INDEX(0, 0)] * v[0]) + (vA[INDEX(0, 1)] * v[1]) + (vA[INDEX(0, 2)] * v[2])) % 256 ;
    w[1] = ((vA[INDEX(1, 0)] * v[0]) + (vA[INDEX(1, 1)] * v[1]) + (vA[INDEX(1, 2)] * v[2])) % 256 ;
    w[2] = ((vA[INDEX(2, 0)] * v[0]) + (vA[INDEX(2, 1)] * v[1]) + (vA[INDEX(2, 2)] * v[2])) % 256 ;

    q[0] = ((vB[INDEX(0, 0)] * w[0]) + (vB[INDEX(0, 1)] * w[1]) + (vB[INDEX(0, 2)] * w[2])) % 256 ;
    q[1] = ((vB[INDEX(1, 0)] * w[0]) + (vB[INDEX(1, 1)] * w[1]) + (vB[INDEX(1, 2)] * w[2])) % 256 ;
    q[2] = ((vB[INDEX(2, 0)] * w[0]) + (vB[INDEX(2, 1)] * w[1]) + (vB[INDEX(2, 2)] * w[2])) % 256 ;
    
    if(q[0] != v[0] || q[1] != v[1] || q[2] != v[2])
    {
      authentic = false ;
    }
    else
    {
      Serial.print("Original:");
      Serial.print(v[0], HEX) ;
      Serial.print("\t") ;
      Serial.print(v[1], HEX) ;
      Serial.print("\t") ;
      Serial.println(v[2], HEX) ;
  
      Serial.print("Crypted :");
      Serial.print(w[0], HEX) ;
      Serial.print("\t") ;
      Serial.print(w[1], HEX) ;
      Serial.print("\t") ;
      Serial.println(w[2], HEX) ;
  
      Serial.print("Decoded :");
      Serial.print(q[0], HEX) ;
      Serial.print("\t") ;
      Serial.print(q[1], HEX) ;
      Serial.print("\t") ;
      Serial.println(q[2], HEX) ;
    }
  }

  return authentic ;
}

//-------------------------//
//----[ I/O FUNCTIONS ]----//
//-------------------------//

void printKey(byte* key)
{
  for (int line = 0; line < N; line++)
  {
     for (int row = 0; row < N; row++)
     {
        Serial.print(key[INDEX(line, row)], HEX) ;
        Serial.print("\t") ;
     }
  }
  Serial.println("") ;
  for (int line = 0; line < N; line++)
  {
     for (int row = 0; row < N; row++)
     {
        Serial.print(key[INDEX(line, row)], DEC) ;
        Serial.print("\t") ;
     }
  }
  Serial.println("") ;
}

