#pragma once
#include "ProtobufModule.h"
#include "concurrency/OSThread.h"

/**
 * Position module for sending/receiving positions into the mesh
 */
class PositionModule : public ProtobufModule<meshtastic_Position>, private concurrency::OSThread
{
    /// The id of the last packet we sent, to allow us to cancel it if we make something fresher
    PacketId prevPacketId = 0;

    /// We limit our GPS broadcasts to a max rate
    uint32_t lastGpsSend = 0;

    // Store the latest good lat / long
    int32_t lastGpsLatitude = 0;
    int32_t lastGpsLongitude = 0;

    /// We force a rebroadcast if the radio settings change
    uint32_t currentGeneration = 0;

  public:
    /** Constructor
     * name is for debugging output
     */
    PositionModule();

    /**
     * Send our position into the mesh
     */
    void sendOurPosition(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false, uint8_t channel = 0);

    void handleNewPosition();

  protected:
    /** Called to handle a particular incoming message

    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Position *p) override;

    /** Messages can be received that have the want_response bit set.  If set, this callback will be invoked
     * so that subclasses can (optionally) send a response back to the original sender.  */
    virtual meshtastic_MeshPacket *allocReply() override;

    /** Does our periodic broadcast */
    virtual int32_t runOnce() override;

  private:
    struct SmartPosition getDistanceTraveledSinceLastSend(meshtastic_PositionLite currentPosition);
    meshtastic_MeshPacket *allocAtakPli();
    uint32_t precision;

    /** Only used in power saving trackers for now */
    void clearPosition();
    void sendLostAndFoundText();
};

struct SmartPosition {
    float distanceTraveled;
    uint32_t distanceThreshold;
    bool hasTraveledOverThreshold;
};

extern PositionModule *positionModule;