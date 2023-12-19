#include <extern/serial/serial.h>
#include <SDL2/SDL_assert.h>
#include "com.hxx"


#include <cctype>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <exception>

std::mutex                      com_mutex                     = std::mutex();
constexpr uint32_t              com_baud_rate                 = 115200; 
constexpr std::string_view      com_coin_prefix               = "@COIN";
constexpr std::string_view      com_coin_suffix               = "&";
std::unique_ptr<serial::Serial> serial_port                   = nullptr;

std::thread         serial_thread;


namespace COM {
    void COMLoop() {
        std::string prefix = "";
        while (serial_port->isOpen()) {
            if (serial_port->available() >= 1) {
                prefix += serial_port->read(1);
                if (prefix.length() > com_coin_prefix.length()) 
                    prefix = prefix.substr(prefix.length()-com_coin_prefix.length());
                if (prefix == com_coin_prefix) {
                    std::string coin_value = "";
                    for (int i = 0; i < 8; i++) {
                        uint8_t x; serial_port->read(&x, 1);
                            if (std::isdigit(x)) coin_value += char(x);
                        if (x == '&') break;
                    };

                    coin_inserted_value += (uint16_t)std::stoi(coin_value);
                }         
            }
        }

    }


    void beginCOMThread(void) {
        if (!com_mutex.try_lock()) return;
        if (serial_thread.joinable()) return;
        if (serial_port)
            if (serial_port->isOpen()) return;
        

        std::string com_port = "COM1";
        if (const char *cp = std::getenv("COM_PORT"))
            com_port = cp;
        serial_port = std::unique_ptr<serial::Serial>(new serial::Serial(com_port, com_baud_rate));
        if (!serial_port->isOpen()) {
            serial_port->open();
            SDL_assert(serial_port->isOpen());
        }
        
        
        coin_inserted_value.store(0);
        serial_port->write("# ~Hello World~! :)");
        com_mutex.unlock();
        serial_thread = std::thread(COMLoop);
    }

    void endCOMThread(void) {
        serial_thread.join();
        serial_port->flush();
        serial_port->close();
        delete serial_port.release();
    }
} // namespace Serial
