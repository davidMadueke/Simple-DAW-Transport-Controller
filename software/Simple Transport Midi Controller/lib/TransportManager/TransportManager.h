#include <MIDI_PACKET.h>

class TransportManager
{
    private:

    public:
    TransportManager();

    void begin();
    void read();

    void send(MIDI_PACKET);

};