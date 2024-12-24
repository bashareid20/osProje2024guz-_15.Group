# İşletim Sistemleri Projesi - Kabuk (Shell) Uygulaması

Bu proje, İşletim Sistemleri dersi kapsamında hazırlanmış bir temel kabuk (shell) uygulamasıdır. Proje, Linux ortamında, C programlama dili kullanılarak geliştirilmiştir.

## Proje Hedefleri
Bu proje ile aşağıdaki konular öğrenilmiş ve uygulanmıştır:
- Proses (süreç) yönetimi
- Giriş/Çıkış (I/O) yönlendirme
- Linux sinyalleri kullanımı
- Pipe (boru) ve noktalı virgül ile komut yürütme
- Arka plan işlemleri

## Özellikler
Proje kapsamında aşağıdaki özellikler hayata geçirilmiştir:
1. **Prompt (Komut istemi)**: Kullanıcıdan komut almak için prompt görüntülenir.
2. **Built-in Komutlar**: `quit` komutu ile kabuk sonlandırılabilir.
3. **Tekli Komut İcrası**: Bağımsız bir komut çalıştırılabilir (örneğin: `ls -l`).
4. **Giriş Yönlendirme**: Komutların giriş verisi bir dosyadan alınabilir (`<` kullanılarak).
5. **Çıkış Yönlendirme**: Komutların çıktısı bir dosyaya yönlendirilebilir (`>` kullanılarak).
6. **Arka Plan İşlemleri**: `&` ile belirtilen komutlar arka planda çalıştırılabilir.
7. **Pipe (Boru)**: Komutlar arasında pipe kullanılarak veri aktarımı sağlanabilir (`|` ile).

## Proje Yapısı
Proje aşağıdaki dosyalardan oluşmaktadır:
- **Kaynak Kodlar**: `*.c` ve `*.h` dosyaları
- **Makefile**: Derleme işlemini otomatikleştirmek için
- **README.md**: Proje hakkında bilgi içeren bu dosya

## Çalıştırma Talimatları
1. **Gereksinimler**:
   - Linux işletim sistemi
   - GCC derleyicisi

2. **Derleme**:
   Projeyi derlemek için aşağıdaki komutu kullanabilirsiniz:
   ```bash
   make
