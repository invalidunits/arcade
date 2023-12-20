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
constexpr char                  com_packet_begin              = '[';
constexpr char                  com_packet_end                = ']';

// coin_inserted_value
constexpr char                  com_coin_prefix               = 'C';


std::unique_ptr<serial::Serial> serial_port                   = nullptr;

std::thread                     serial_thread;
std::atomic<bool>               cancel_com;

namespace COM {
    void COMLoop() {
        std::string current_packet = "";
        while (serial_port->isOpen() && !cancel_com) {
            if (serial_port->available() >= 1) {
                auto data = serial_port->read(1);
                if (data[0] == com_packet_begin) {
                    current_packet = "";
                    continue;
                }

                if (data[0] == com_packet_end) {
                    // Interpret packet.

                    if (current_packet[0] == com_coin_prefix) {
                        int value = 0;
                        try  {
                            value += std::stoi(current_packet.substr(1));
                        } catch (...) {}
                        COM::coin_inserted_value.fetch_add(value, std::memory_order::memory_order_relaxed);
                    }

                }
                current_packet += data;
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
        com_mutex.unlock();
        serial_thread = std::thread(COMLoop);
    }

    void endCOMThread(void) {
        cancel_com = true;
        serial_thread.join();
        serial_port->flush();
        serial_port->close();
        delete serial_port.release();
    }
} // namespace Serial
