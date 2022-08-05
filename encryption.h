#ifndef ENCRYPTION_H
#define ENCRYPTION_H

static constexpr std::array<char,64> b64 {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z', '0', '1', '2', '3',
   '4', '5', '6', '7', '8', '9', '+', '/'
};

static constexpr std::array<unsigned char,128> numbers {

      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,        //   0 - 15   , , , , , , , , , , , , , , , ,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,        //  16 - 31   , , , , , , , , , , , , , , , ,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,        //  32 - 47   , , , , , , , , , , ,+, , , ,/,
       52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,        //  48 - 63  0,1,2,3,4,5,6,7,8,9, , , , , , ,
      255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,        //  64 - 79   ,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,
       15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,        //  80 - 95  P,Q,R,S,T,U,V,W,X,Y,Z, , , , , ,
      255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,        //  96 - 111  ,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,
       41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255         // 112 - 127 p,q,r,s,t,u,v,w,x,y,z, , , , ,

};

template <typename T>
std::string flatten(const T& in) {
    std::string out {};
    for ( typename T::const_iterator it = in.begin(); it != in.end(); ++it) {
        out.push_back(*it);
    }
    return out;
}

// Encodieren von 8 Byte nach 6 Byte

template<typename T, typename U>
void encode (T& out, const U& plainvec ) {

    // Wir lassen nur std::string, char, unsigned char, uint8_t und std::byte zu.

    static_assert(
             std::disjunction_v<
                     std::is_same<
                        std::vector<char>,T>,
                     std::is_same<
                        std::vector<unsigned char>,T>,
                     std::is_same<
                        std::vector<std::byte>,T>,
                     std::is_same<
                        std::vector<uint8_t>,T>,
                     std::is_same<
                        std::string,T>
             >, "Nur  char, unsigned char, uint8_t, std::byte und std::string sind als Output erlaubt.");

    static_assert(
             std::disjunction_v<
                     std::is_same<
                        std::vector<char>,U>,
                     std::is_same<
                        std::vector<unsigned char>,U>,
                     std::is_same<
                        std::vector<std::byte>,U>,
                     std::is_same<
                        std::vector<uint8_t>,U>,
                     std::is_same<
                        std::string,U>
             >, "Nur  char, unsigned char, uint8_t, std::byte und std::string sind als Input erlaubt.");


    static_assert( sizeof(std::decay_t<typename U::value_type>) == sizeof(std::byte),
    "Der verwendete Typ muss gleich groß wie std::byte sein." );

    static_assert( sizeof(std::decay_t<typename T::value_type>) == sizeof(std::byte),
    "Der verwendete Typ muss gleich groß wie std::byte sein." );

    out.clear();


    std::vector<std::byte> plainbytes {};

    std::transform(plainvec.begin(), plainvec.end(),
                 std::back_inserter(plainbytes),
                 [] (typename U::value_type c)->std::byte {return static_cast<std::byte>(c);});

    size_t inlen = plainbytes.size();

    size_t outlen  = inlen;
   
    size_t padding_len = 0;


    while(( outlen % 3 ) != 0) {
        ++padding_len;
        ++outlen;
    }

    outlen = 4 * outlen / 3;

    out.reserve(outlen);

    for (size_t i = 0; i < outlen/4; ++i) {

        const size_t idx = i*3;

        std::byte b0 {0};
        std::byte b1 {0};
        std::byte b2 {0};

        if (idx+0 < inlen) {
            b0 = plainbytes[idx+0];
        }
        if (idx+1 < inlen) {
            b1 = plainbytes[idx+1];
        }
        if (idx+2 < inlen) {
            b2 = plainbytes[idx+2];
        }

        const std::byte nb0 =                       ((b0 & std::byte(0xFC)) >> 2);


        const std::byte nb1 = ((b0 & std::byte(0x03)) << 4) | ((b1 & std::byte(0xF0)) >> 4);

      
        const std::byte nb2 = ((b1 & std::byte(0x0F)) << 2) |  ((b2 & std::byte(0xC0)) >> 6);

        const std::byte nb3 = ((b2 & std::byte(0x3F)) << 0);       

        out.push_back(b64[std::to_integer<uint8_t>(nb0)]);
        out.push_back(b64[std::to_integer<uint8_t>(nb1)]);
        out.push_back(b64[std::to_integer<uint8_t>(nb2)]);
        out.push_back(b64[std::to_integer<uint8_t>(nb3)]);
    }



    for (size_t j = 0; j < padding_len; ++j ) {
        out.at(outlen -j - 1) = '=';
    }
}

// Dekodieren von Base64 nach Binär

template<typename T, typename U>
void decode( T& out, const U& encoded ) {

    // Wir lassen nur std::string, char, unsigned char, uint8_t und std::byte zu.

    static_assert(
             std::disjunction_v<
                     std::is_same<
                        std::vector<char>,T>,
                     std::is_same<
                        std::vector<unsigned char>,T>,
                     std::is_same<
                        std::vector<std::byte>,T>,
                     std::is_same<
                        std::vector<uint8_t>,T>,
                     std::is_same<
                        std::string,T>
             >, "Nur  char, unsigned char, uint8_t, std::byte und std::string sind als Output erlaubt.");


    static_assert(
             std::disjunction_v<
                     std::is_same<
                        std::vector<char>,U>,
                     std::is_same<
                        std::vector<unsigned char>,U>,
                     std::is_same<
                        std::vector<std::byte>,U>,
                     std::is_same<
                        std::vector<uint8_t>,U>,
                     std::is_same<
                        std::string,U>
             >, "Nur  char, unsigned char, uint8_t, std::byte und std::string sind als Input erlaubt.");


    static_assert( sizeof(std::decay_t<typename U::value_type>) == sizeof(std::byte),
    "Der verwendete Typ muss gleich groß wie std::byte sein." );

    static_assert( sizeof(std::decay_t<typename T::value_type>) == sizeof(std::byte),
    "Der verwendete Typ muss gleich groß wie std::byte sein." );

    std::vector<std::byte> plainbytes {};

    std::transform(encoded.begin(), encoded.end(),
                 std::back_inserter(plainbytes),
                 [] (typename U::value_type c)->std::byte {return static_cast<std::byte>(c);});

    size_t in_size = plainbytes.size();
    
    size_t enc_size = in_size;          // Startwert
    while((enc_size % 4) != 0 ) {
       ++enc_size;                      // Verlängern
    }

    out.reserve( 3 * enc_size / 4 );

    for (size_t i = 0; i < enc_size; i += 4 ) {
        
        std::byte b0 {0xFF};        // 11111111
        std::byte b1 {0xFF};        // 11111111
        std::byte b2 {0xFF};        // 11111111
        std::byte b3 {0xFF};        // 11111111

        // Beispiel : QUJD Base64-kodiert.
       
        // Überprüfung auf Buchstaben, Zahlen und '+' sowie '/'
        if(                  encoded[i+0] <= 'z' ) {

            b0 = std::byte{numbers[encoded[i+0]]};
        }
        if( i+1 < in_size && encoded[i+1] <= 'z' ) {

            b1 = std::byte{numbers[encoded[i+1]]};
        }
        if( i+2 < in_size && encoded[i+2] <= 'z' ) {

            b2 = std::byte{numbers[encoded[i+2]]};
        }
        if( i+3 < in_size && encoded[i+3] <= 'z' ) {

            b3 = std::byte{numbers[encoded[i+3]]};
        }

        const std::byte nb0 {((b0 & std::byte(0x3F)) << 2) | ((b1 & std::byte(0x30)) >> 4 )};

        const std::byte nb1 {((b1 & std::byte(0x0F)) << 4) | ((b2 & std::byte(0x3C)) >> 2 )};

        const std::byte nb2 {((b2 & std::byte(0x03)) << 6) | ((b3 & std::byte(0x3F)) >> 0 )};

        if( b1 != std::byte(0xFF)) { out.push_back(static_cast<typename U::value_type>(nb0));}
        if( b2 != std::byte(0xFF)) { out.push_back(static_cast<typename U::value_type>(nb1));}
        if( b3 != std::byte(0xFF)) { out.push_back(static_cast<typename U::value_type>(nb2));}
    }
}

#endif
