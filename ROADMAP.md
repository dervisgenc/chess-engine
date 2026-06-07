# Chess Engine — Geliştirme Yol Haritası

## Faz 0: Mevcut Bugları Düzelt

### 0.1 — `Board::occupancy(Color)` sonsuz döngü
`board.cpp` satır 422: `for (; start < start + 6; start++)` koşulu her zaman true.
`start + 6` ifadesi her iterasyonda start ile birlikte artıyor.
Döngüye girmeden önce sınır bir değişkene kaydedilmeli: `int end = start + 6`.

### 0.2 — FEN parse'da NO_PIECE bitboard'a yazılıyor
`board.cpp` satır 96: Rakam karakterlerinde `piece = NO_PIECE` atanıyor ama
bitboard set işlemi switch dışında koşulsuz çalışıyor. `m_bitboards[0]`'a gereksiz
bitler yazılıyor. Digit durumunda `continue` kullanılmalı veya set işlemi
`if (piece != NO_PIECE)` koşuluna alınmalı.

### 0.3 — `board.h` header guard eksik
`#ifndef BOARD_H` var ama `#define BOARD_H` satırı yok.

### 0.4 — Piyon atak tablosu initialize edilmiyor
`pawn_attacks[2][64]` tanımlı ama `init_attacks()` içinde doldurulmuyor.

---

## Faz 1: Move Temsili

Bir hamleyi temsil etmek için bir veri yapısına ihtiyacımız var. Bu yapı motorun
en temel iletişim birimi — hamle üretimi, arama, make/unmake hepsi bu yapıyı
kullanacak. Dolayısıyla tasarım kararları motorun geri kalanını doğrudan etkiler.

### 1.1 — MoveFlag enum tanımla

**Neden flag gerekiyor?**

Satrançta bazı hamleler "özel" kurallara sahip. Sadece `from` ve `to` bilgisi
bu hamleleri ayırt etmeye yetmez:

- e1→g1 normal bir şah hamlesi mi, yoksa kısa rok mu? Rokta kale de hareket eder.
- e5→d6 normal bir piyon yemesi mi, yoksa en passant mı? En passant'ta yenen
  piyon d6'da değil d5'te duruyor.
- e7→e8 normal bir piyon ilerlemesi mi? Hayır, piyonun promote olması gerekiyor.
  Ama hangi taşa? At mı, vezir mi?

Flag olmadan `make_move` fonksiyonu bu durumları ayırt edemez. Her hamlenin
türünü bilmemiz gerekiyor ki tahtayı doğru güncelleyebilelim.

**Flag değerleri:**

```
QUIET                    — Normal, sessiz hamle (yeme yok, özel durum yok)
DOUBLE_PAWN_PUSH         — Piyon 2 kare ilerleme (en passant karesini set etmek için lazım)
KING_CASTLE              — Kısa rok (kaleyi de taşımak için)
QUEEN_CASTLE             — Uzun rok
CAPTURE                  — Normal yeme (hedef karedeki taşı kaldırmak için)
EN_PASSANT               — En passant (yenen piyon to karesinde değil, farklı yerde)
KNIGHT_PROMOTION         — Piyon → At (yemesiz)
BISHOP_PROMOTION         — Piyon → Fil
ROOK_PROMOTION           — Piyon → Kale
QUEEN_PROMOTION          — Piyon → Vezir
KNIGHT_PROMOTION_CAPTURE — Piyon → At (yemeli)
BISHOP_PROMOTION_CAPTURE — Piyon → Fil (yemeli)
ROOK_PROMOTION_CAPTURE   — Piyon → Kale (yemeli)
QUEEN_PROMOTION_CAPTURE  — Piyon → Vezir (yemeli)
```

**Neden CAPTURE ve QUIET ayrı?**

make_move sırasında "hedef karede taş var mı?" diye tekrar board'a bakmak
yerine, flag'e bakarak anında biliyoruz. Ayrıca search aşamasında hamle
sıralaması (move ordering) için capture hamleleri quiet hamlelerden önce
denemek isteyeceğiz — flag sayesinde O(1)'de ayırt edebiliriz.

**Neden promotion × capture = 8 ayrı flag?**

Alternatif: flag'leri bit flag olarak tasarlayıp `PROMOTION | CAPTURE | QUEEN`
gibi OR'lamak. Ama bu durumda flag'den promotion taşını çıkarmak için maskeleme
gerekiyor. Ayrı enum değerleri daha okunabilir ve switch-case ile doğrudan
eşleşiyor. Toplam 14 değer bir uint8_t'ye rahat sığıyor.

### 1.2 — Move struct oluştur

```
struct Move {
    Square from;      // 1 byte — kaynak kare (0-63)
    Square to;        // 1 byte — hedef kare (0-63)
    MoveFlag flag;    // 1 byte — hamle türü
};
```

**Neden struct, neden class değil?**

- Move sadece veri taşıyor. Kendi başına bir "davranışı" yok — hamleyi
  uygulayan Board sınıfı. Move "ne yapılacağını" tanımlıyor, "nasıl
  yapılacağını" değil.
- Tüm alanlar doğrudan okunacak, encapsulation (private + getter/setter)
  gereksiz karmaşıklık ekler.
- `move.from` yazmak `move.get_from()` yazmaktan hem daha okunabilir
  hem de kavramsal olarak daha dürüst — gizlenecek bir şey yok.

**Neden Stockfish gibi 16-bit packed integer değil?**

Stockfish'in yaklaşımı (2 byte):
```
15-14: type   13-12: promo   11-6: from   5-0: to
```

Bizim yaklaşımımız (3 byte):
```
from (uint8) | to (uint8) | flag (uint8)
```

1 byte fark. Ama bu 1 byte karşılığında:
- Kod çok daha okunabilir — `move.from` vs `(data >> 6) & 0x3F`
- Debug çok daha kolay — debugger'da struct alanlarını doğrudan görürsün
- Hata yapma ihtimali düşük — bit shifting hatası yok

Bu fark search derinliğini veya hızı ölçülebilir şekilde etkilemez. Darboğaz
hamle temsili değil, arama algoritması ve değerlendirme fonksiyonu olacak.
İleride profiling yaptıktan sonra packed versiyona geçiş bir refactor mesafesinde.

**Board'da hangi taşın yendiğini neden Move'da tutmuyoruz?**

Bazı motorlar `captured_piece` alanı ekler. Biz eklemiyoruz çünkü:
- make_move sırasında Board zaten hedef karedeki taşı biliyor
- Copy-make yaklaşımında unmake'e gerek yok (eski board'u geri yüklersin)
- Move'u 3 byte'ta tutmak, arama sırasında cache-friendly

### 1.3 — MoveList struct oluştur

**Neden bir yapıya ihtiyacımız var?**

Hamle üretimi bir dizi hamle döndürüyor. Bu hamleleri tutacak bir konteyner lazım.

```
struct MoveList {
    Move moves[256];
    int count = 0;

    void add(Move m) { moves[count++] = m; }
};
```

**Neden 256?**

Bir satranç pozisyonunda teorik maximum legal hamle sayısı 218 (bilinen en
yüksek). Pseudo-legal üretimde biraz daha fazla olabilir. 256 = 2^8, güvenli
bir üst sınır ve bellek hizalaması (alignment) için uygun.

**Neden `std::vector<Move>` değil?**

Search sırasında her derinlikte `generate_moves` çağrılır. Derinlik 10'da
bu milyonlarca çağrı demek. Her çağrıda `vector`:
1. Heap'ten bellek ayırır (malloc)
2. Move'lar eklenirken büyüyüp realloc yapar
3. Fonksiyon bitince belleği geri verir (free)

malloc/free system call'ları, stack allocation'a kıyasla çok pahalı.
Sabit boyutlu dizi stack'te yaşar — allocation maliyeti sıfır.

**Neden pointer + count yerine struct?**

Düz bir `Move moves[256]` dizisi ve ayrı bir `int count` da çalışır. Ama
struct olarak gruplamak:
- İkisini birlikte taşımayı garanti eder (fonksiyona tek parametre)
- `add()` metodu count artırmayı unutma hatasını önler
- Mantıksal birlik: "bir pozisyondaki hamleler" tek bir kavram

---

## Faz 2: Piyon Atak Tablosu

### 2.1 — Atak vs Hamle: Önemli Ayrım

Piyon, satrançta hareket ettiği yönle atak yaptığı yönün **farklı** olduğu
tek taş. Bu ayrımı anlamak kritik:

```
Piyon hareketi:     Piyon atağı:
    .                 x . x
    P                   P
```

- **Hareket**: Düz ileri (beyaz için north, siyah için south). Yeme yapamaz.
- **Atak**: Çapraz ileri (north_east/north_west veya south_east/south_west).
  Sadece orada rakip taş varsa hamle üretilir.

Bu yüzden piyon atak tablosu sadece **çapraz atakları** tutar. İleri hareket
hamle üretiminde ayrıca ele alınır (Faz 3.6).

### 2.2 — Neden lookup table?

At ve şah için nasıl `knight_attacks[64]` ve `king_attacks[64]` tabloları
varsa, piyon için de `pawn_attacks[2][64]` tablosu oluşturuyoruz.

Neden `[2]`? Çünkü piyon renke bağlı hareket eden tek taş:
- `pawn_attacks[WHITE][sq]`: Beyaz piyonun sq karesinden atak yaptığı kareler
- `pawn_attacks[BLACK][sq]`: Siyah piyonun sq karesinden atak yaptığı kareler

Neden runtime'da hesaplamıyoruz? Aynı kareye milyonlarca kez bakılacak.
Bir kere hesaplayıp tabloda tutmak O(1) lookup sağlıyor.

### 2.3 — `generate_pawn_attacks` fonksiyonunu yaz

Her kare için o karedeki piyonun atak ettiği karelerin bitboard'ını döndür.

**Beyaz piyon (sq karesinden):**
```
atak = north_east(1ULL << sq) | north_west(1ULL << sq)
```

**Siyah piyon (sq karesinden):**
```
atak = south_east(1ULL << sq) | south_west(1ULL << sq)
```

Direction fonksiyonları zaten file mask kontrolünü yapıyor (`not_a_file`,
`not_h_file`), bu yüzden a-file'daki bir piyon sola taşmaz, h-file'daki
bir piyon sağa taşmaz.

**Kenar durumlar:**
- 1. sıradaki beyaz piyon: Pratikte olamaz, ama tablo 64 kare için hesaplanır.
  north_east/north_west doğru sonuç verir, sorun olmaz.
- 8. sıradaki siyah piyon: Aynı şekilde.

### 2.4 — `init_attacks()` içinde piyon tablosunu doldur

Mevcut `init_attacks()` fonksiyonuna ekleme:

```
for sq = A1 to H8:
    pawn_attacks[WHITE][sq] = generate_pawn_attacks(WHITE, sq)
    pawn_attacks[BLACK][sq] = generate_pawn_attacks(BLACK, sq)
```

Fonksiyon imzası iki şekilde olabilir:
- `generate_pawn_attacks(Color side, Square sq)` — tek fonksiyon, rengi parametre al
- `generate_white_pawn_attacks(sq)` / `generate_black_pawn_attacks(sq)` — ayrı fonksiyonlar

Tek fonksiyon daha temiz. İçeride `if (side == WHITE)` ile yön seçimi yapılır.

---

## Faz 3: Pseudo-Legal Hamle Üretimi

### Neden "pseudo-legal" ve neden "legal" değil?

Doğrudan legal hamle üretmenin iki yolu var:

**Yol A — Üretirken filtrele:**
Her hamle üretilirken pin, check, absolute pin analizleri yap.
Sadece legal hamleler listede olsun.
→ Karmaşık, hata eğilimli, her taş için özel pin mantığı gerekiyor.

**Yol B — Üret, dene, filtrele (pseudo-legal yaklaşım):**
1. Tüm hamleleri pin/check düşünmeden üret (pseudo-legal)
2. Her hamleyi `make_move` ile uygula
3. Kendi şahın tehdit altında mı kontrol et
4. Tehdit altındaysa hamle illegal — geri al

→ Basit, modüler, hata yapma ihtimali düşük.

Yol B daha yavaş gibi görünebilir ama:
- Çoğu pozisyonda pseudo-legal hamlelerin büyük kısmı zaten legal
- `make_move` + `is_attacked` çok hızlı işlemler (bitwise)
- Kod karmaşıklığı düşük = daha az bug = perft'i daha hızlı geçersin

Stockfish dahil birçok motor Yol B'nin varyantlarını kullanır. Biz de
bununla başlayacağız.

### Ortak Pattern: Bitscan Döngüsü

Tüm taş üretimleri aynı temel kalıbı izler. Bu kalıp bitboard
programlamanın en temel idiom'u:

```
pieces = board'dan aktif tarafın [taş] bitboard'ı  // örn: beyaz atlar
while (pieces) {
    sq = bitscan_forward(pieces)    // en düşük set bit → kare indeksi
    pieces &= pieces - 1           // o biti temizle (pop LSB)

    targets = atak_tablosu(sq)      // bu taşın atak ettiği kareler
    targets &= ~friendly_occupancy  // kendi taşlarını çıkar

    captures = targets & enemy_occupancy   // rakip taşı olan kareler
    quiets   = targets & ~all_occupancy    // boş kareler

    while (captures) {
        to = bitscan_forward(captures)
        captures &= captures - 1
        movelist.add(Move{sq, to, CAPTURE})
    }
    while (quiets) {
        to = bitscan_forward(quiets)
        quiets &= quiets - 1
        movelist.add(Move{sq, to, QUIET})
    }
}
```

**`pieces &= pieces - 1` nedir?**

Bu, bir bitboard'dan en düşük set bit'i temizlemenin standart yolu.
Örnek: `pieces = 0b1010`, `pieces - 1 = 0b1001`, AND → `0b1000`.
En alttaki 1 silindi. Buna "pop LSB" denir.

Bu döngüde branch veya array index yok — tamamen bitwise. Bu yüzden
bitboard temsili bu tür işlemlerde çok verimli.

### 3.1 — At hamle üretimi (`generate_knight_moves`)

**Neden ilk sırada?**

At en basit taş çünkü:
- Lookup tablosu zaten hazır (`knight_attacks[64]`)
- Kayan taş değil — occupancy'ye bağlı değil
- Özel durumu yok (promotion, en passant, rok gibi)

Bu yüzden ortak pattern'i ilk kez at ile implement etmek mantıklı.
Pattern çalıştığında şah, kale, fil, vezir aynı kalıba oturacak.

**Fonksiyon imzası:**

```
void generate_knight_moves(const Board& board, Color side, MoveList& list)
```

Neden `const Board&`? Board'u kopyalamak istemiyoruz (16 bitboard = 128 byte),
sadece okuyoruz. Neden referans? Pointer da olur ama referans "null olamaz"
garantisi verir ve `board.occupancy()` syntax'ı `board->occupancy()`'den
daha temiz.

**Adımlar:**

1. `board.get_bitboard(side == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT)` ile
   at bitboard'ını al. (Not: Board'a bir getter lazım, şu an bitboard'lar private.)
2. `board.occupancy(side)` ile kendi taşlarının bitboard'ını al → `friendly`
3. `board.occupancy(opposite_side)` ile rakip taşları al → `enemy`
4. Bitscan döngüsü ile her at için:
   - `targets = knight_attacks[sq] & ~friendly`
   - `captures = targets & enemy` → her biri CAPTURE
   - `quiets = targets & ~(friendly | enemy)` → her biri QUIET

### 3.2 — Şah hamle üretimi (`generate_king_moves`)

At ile birebir aynı pattern. Tek fark:
- `king_attacks[sq]` tablosu kullanılır
- Tahtada her zaman sadece 1 şah var, döngü her zaman 1 iterasyon çalışır
- Rok hamleleri burada ele alınmaz — ayrı fonksiyonda (3.7)

### 3.3 — Kale hamle üretimi (`generate_rook_moves`)

**At/şahtan farkı:** Kale bir kayan taş (sliding piece). Atak bitboard'ı
sadece kareye değil, **occupancy'ye** de bağlı.

- At: `knight_attacks[sq]` → occupancy bilgisine gerek yok
- Kale: `rook_attacks(sq, occupancy)` → tahtadaki tüm taşların konumuna bağlı

`rook_attacks` fonksiyonun zaten `movegen.cpp`'de mevcut. Bu fonksiyon ray
tabloları ve blocker tespiti ile doğru atakları hesaplıyor.

**Adımlar:**

1. Kale bitboard'ını al
2. Toplam occupancy'yi al (`board.occupancy()`)
3. Her kale için:
   - `targets = rook_attacks(sq, occupancy) & ~friendly`
   - captures ve quiets olarak ayır, listeye ekle

### 3.4 — Fil hamle üretimi (`generate_bishop_moves`)

Kale ile birebir aynı pattern. `bishop_attacks(sq, occupancy)` kullanılır.
Çapraz yönlerde (NE, NW, SE, SW) blocker tespiti yapılır.

### 3.5 — Vezir hamle üretimi (`generate_queen_moves`)

Yine aynı pattern. `queen_attacks(sq, occupancy)` zaten
`rook_attacks | bishop_attacks` olarak implement edilmiş durumda.

Vezir = kale + fil. Ayrı bir atak mantığı yok.

### 3.6 — Piyon hamle üretimi (`generate_pawn_moves`)

**Neden en sona bıraktık?**

Piyon satrançta en karmaşık hamle kurallarına sahip taş:
- İleri hareket (1 kare) — ama sadece hedef boşsa (diğer taşlar her zaman
  hedef kareye gidebilir)
- Çift ilerleme (2 kare) — sadece başlangıç sırasından, ve aradaki kare de
  boş olmalı
- Çapraz yeme — ileri hareket yönünden farklı
- En passant — özel bir yeme türü, hedef karedeki taş değil yanındaki taş yenir
- Promotion — 8. (veya 1.) sıraya ulaşınca 4 farklı taşa dönüşebilir
- Promotion + capture — ikisinin kombinasyonu

Diğer 5 taş hep aynı kalıbı izliyor: "atak tablosundan hedefleri al, filtrele,
ekle." Piyon bu kalıba uymaz, her alt-durum ayrı bitwise mantık gerektirir.

**İleri hareket (tek kare):**

Beyaz için: piyonları 8 bit sola kaydır (north), occupancy ile AND-NOT
yaparak sadece boş kareleri tut.

```
single_push = north(white_pawns) & ~occupancy
```

Neden atak tablosu kullanmıyoruz? Çünkü atak tablosu çapraz yönleri tutuyor,
ileri hareket bir atak değil.

Neden her piyon için ayrı ayrı değil de hepsini birden kaydırıyoruz? Çünkü
shift işlemi tüm piyonlara aynı anda uygulanır — bitboard'ın gücü bu.
64 kareyi tek bir shift ile işliyoruz.

**Çift ilerleme:**

```
double_push = north(single_push) & ~occupancy & rank_4_mask
```

Neden `single_push`'tan başlıyoruz? Çünkü aradaki karenin de boş olması
gerekiyor. `single_push` zaten bu kontrolü içeriyor. Sadece rank_4'e (beyaz
için) veya rank_5'e (siyah için) ulaşanları filtreleriz.

**Çapraz yeme:**

```
left_captures  = north_west(white_pawns) & enemy
right_captures = north_east(white_pawns) & enemy
```

Burada `pawn_attacks` tablosunu kullanmak yerine toplu shift yapıyoruz.
Aynı sonuç, ama tüm piyonlar için tek seferde.

**En passant:**

Board'dan `en_passant_square` bilgisini al. Eğer SQ_NONE değilse:
piyonlarımızdan bu kareye çapraz atak edebilen var mı kontrol et.

```
ep_sq = board.en_passant_square()
eğer ep_sq != SQ_NONE:
    attackers = pawn_attacks[enemy_side][ep_sq] & our_pawns
    // attackers'daki her piyon → Move{sq, ep_sq, EN_PASSANT}
```

Burada **ters yönde** atak tablosu kullanımına dikkat: "ep karesine hangi
piyonlarımız saldırabilir?" sorusunu sormak için rakip tarafın atak tablosunu
kullanıyoruz. Çünkü beyaz piyonun ep_sq'ye çapraz atağı, siyah piyonun
ep_sq'den çapraz atağının simetrisi.

**Promotion:**

`single_push` veya capture sonucu 8. sıraya (beyaz) veya 1. sıraya (siyah)
ulaşan her piyon için 4 hamle üret:

```
promotions = single_push & rank_8_mask
her hedef kare için:
    movelist.add(Move{from, to, QUEEN_PROMOTION})
    movelist.add(Move{from, to, ROOK_PROMOTION})
    movelist.add(Move{from, to, BISHOP_PROMOTION})
    movelist.add(Move{from, to, KNIGHT_PROMOTION})
```

Neden 4 hamle? Kurallar gereği oyuncu istediği taşı seçer. Motor 4'ünü de
denemelidir. Pratikte vezir promotion neredeyse her zaman en iyisidir ama
at promotion ile mat veren pozisyonlar var (underpromotion).

Promotion + capture da aynı mantık, sadece flag QUEEN_PROMOTION_CAPTURE vb. olur.

**`from` karesini nasıl buluyoruz?**

Toplu shift yaptığımızda sonuç bitboard'ı `to` karelerini veriyor. `from`
karesini bulmak için ters shift:
- Beyaz single push: `from = to - 8`
- Beyaz double push: `from = to - 16`
- Beyaz left capture: `from = to - 7`
- Beyaz right capture: `from = to - 9`

### 3.7 — Rok hamle üretimi (`generate_castling_moves`)

**Kontrol sırası (kısa devre mantığıyla):**

1. `board.castling_rights() & WHITE_CASTLING_00` → hak var mı?
2. `occupancy & between_mask` → aradaki kareler boş mu?
   - Beyaz kısa rok: f1, g1 boş olmalı
   - Beyaz uzun rok: b1, c1, d1 boş olmalı
3. Şah şu an check'te mi? (Check'teyken rok yapılamaz)
4. Geçiş kareleri tehdit altında mı? (is_square_attacked gerekecek)

2 numaralı kontrol saf bitwise: `if (!(occupancy & between_mask))`.
3 ve 4 numaralı kontroller `is_square_attacked` fonksiyonunu gerektirir
ki bu Faz 5.2'de implement edilecek. Bu yüzden ilk aşamada rok üretimini
basitleştirmek (sadece 1-2 kontrolü yapmak) veya Faz 5'e ertelemek
değerlendirilebilir.

**`between_mask` sabitleri:**

```
WHITE_OO_MASK  = (1ULL << SQ_F1) | (1ULL << SQ_G1)
WHITE_OOO_MASK = (1ULL << SQ_B1) | (1ULL << SQ_C1) | (1ULL << SQ_D1)
BLACK_OO_MASK  = (1ULL << SQ_F8) | (1ULL << SQ_G8)
BLACK_OOO_MASK = (1ULL << SQ_B8) | (1ULL << SQ_C8) | (1ULL << SQ_D8)
```

### 3.8 — Ana `generate_moves` fonksiyonu

Tüm alt-fonksiyonları çağıran giriş noktası:

```
void generate_moves(const Board& board, MoveList& list) {
    Color side = board.side_to_move();

    generate_pawn_moves(board, side, list);
    generate_knight_moves(board, side, list);
    generate_bishop_moves(board, side, list);
    generate_rook_moves(board, side, list);
    generate_queen_moves(board, side, list);
    generate_king_moves(board, side, list);
    generate_castling_moves(board, side, list);
}
```

**Neden tek bir MoveList referans olarak geçiriliyor?**

Her fonksiyon ayrı liste döndürüp sonra birleştirmek kopya yaratır.
Aynı listeye referans geçirerek tüm hamleler tek dizide toplanır.

**Neden Board referansı?**

Hamle üretimi board'u değiştirmez, sadece okur. `const Board&` ile
bunu garanti ediyoruz. Board private member'lara sahip olduğu için
movegen'in ihtiyaç duyduğu bilgilere erişmek için Board'a getter
fonksiyonları eklememiz gerekecek:

- `get_bitboard(Piece p)` veya `bitboard(Piece p)`
- `occupancy()` — zaten mevcut
- `occupancy(Color)` — zaten mevcut (bug düzeltildikten sonra)
- `side_to_move()`
- `castling_rights()`
- `en_passant_square()`

Bu getter'lar Board'un iç yapısını dışarıya en az düzeyde açar.
Bitboard'ları doğrudan public yapmak yerine getter kullanmak, ileride
iç temsili değiştirme esnekliği verir.

### 3.9 — Board'a gerekli getter'ları ekle

Movegen'in Board'dan veri okuması için minimal public interface:

```
// board.h — public bölümüne eklenecek
uint64_t bitboard(Piece p) const { return m_bitboards[p]; }
Color side_to_move() const { return m_side_to_move; }
Castling_Rights castling_rights() const { return m_castling_rights; }
Square en_passant_square() const { return m_en_passant_square; }
```

Bunlar inline ve const — performans maliyeti sıfır, encapsulation korunuyor.

---

## Faz 4: Make / Unmake Move

### 4.1 — `Board::make_move(Move)`
Hamleyi tahtaya uygula:
- Taşı kaynak kareden kaldır, hedef kareye koy
- Capture ise rakip taşı kaldır
- Castling ise kaleyi de taşı
- En passant ise yenen piyonu doğru kareden kaldır
- Promotion ise piyonu sil, yeni taşı koy
- Rok haklarını güncelle (şah veya kale hareket ettiyse)
- En passant karesini güncelle (double push ise)
- Sıra değiştir, sayaçları güncelle

### 4.2 — `Board::unmake_move(Move)`
Hamleyi geri al. İki yaklaşım var:
- **Copy-make**: make'den önce board'un kopyasını al, unmake yerine kopyayı geri yükle (basit)
- **Incremental unmake**: Her değişikliği tek tek geri al (daha hızlı, daha karmaşık)
Başlangıç için copy-make yaklaşımı önerilir.

---

## Faz 5: Legal Move Generation + Perft

### 5.1 — Legal hamle filtresi
Pseudo-legal hamleyi make et, sonra kendi şahının tehdit altında olup olmadığını kontrol et.
Tehdit altındaysa hamle illegal — unmake et ve listeye ekleme.

### 5.2 — `is_square_attacked(Square, Color)` fonksiyonu
Bir karenin belirli bir renk tarafından tehdit altında olup olmadığını kontrol et.
Her taş türü için o kareden "geriye doğru" atak kontrolü yap.

### 5.3 — Perft testi
Bilinen pozisyonlarda hamle sayılarını doğrula. Başlangıç pozisyonu için:
- Depth 1: 20
- Depth 2: 400
- Depth 3: 8.902
- Depth 4: 197.281
- Depth 5: 4.865.609

Bu test motorun doğruluğunu garanti eder. Perft geçmeden search'e geçilmemeli.

---

## Faz 6: Değerlendirme (Evaluation)

### 6.1 — Materyal değerlendirme
Piyon=100, At=320, Fil=330, Kale=500, Vezir=900

### 6.2 — Piece-Square Tables (PST)
Her taş ve her kare için bonus/ceza değerleri. Taşların iyi pozisyonlarda
olmasını teşvik eder (örn: atlar merkezde, piyonlar ileride).

---

## Faz 7: Arama (Search)

### 7.1 — Negamax
Temel minimax algoritmasının negamax formülasyonu.

### 7.2 — Alpha-Beta Pruning
Gereksiz dalları budayarak arama alanını daralt.

### 7.3 — Iterative Deepening
Artan derinliklerle arama yap. Zaman yönetimi için gerekli.

### 7.4 — Quiescence Search
Yaprak düğümlerde capture zincirleri bitene kadar aramaya devam et.
"Horizon effect"i önler.

---

## Faz 8: UCI Protokolü

### 8.1 — Temel UCI komutları
- `uci` → motor bilgisi
- `isready` → `readyok`
- `position` → FEN veya startpos + hamleler
- `go` → arama başlat
- `stop` → aramayı durdur
- `quit` → çıkış

UCI implementasyonu motorun GUI'lerle (Arena, CuteChess vb.) iletişim kurmasını sağlar.

---

## Stockfish Move Referansı

Stockfish'te Move bir **class** olarak tanımlanmış ama aslında sadece bir `uint16_t`
wrapper'ı. OOP anlamında ağır bir class değil — bitwise operasyonları metodlar
arkasına gizleyen ince bir katman:

```
Bit düzeni (16 bit):
  15-14: MoveType (NORMAL=0, PROMOTION=1, EN_PASSANT=2, CASTLING=3)
  13-12: Promotion taşı (KNIGHT=0, BISHOP=1, ROOK=2, QUEEN=3)
  11-6:  From square (0-63)
  5-0:   To square (0-63)
```

Stockfish'in class kullanma sebebi: `from_sq()`, `to_sq()`, `type_of()` gibi
decode metodlarını move ile birlikte tutmak ve `==`, `!=` operatörlerini
overload etmek. Ama altta yatan yapı düz bir 16-bit integer.

Bizim yaklaşım (basit struct) ile Stockfish yaklaşımı (packed class) arasındaki
fark: sizeof(Move) bizde 3 byte (from + to + flag), Stockfish'te 2 byte.
Fonksiyonel olarak aynı işi yaparlar. İleride optimize etmek istersek
packed versiyona geçiş kolay.
