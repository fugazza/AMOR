#include <Arduino.h>
#include <opentherm.h>
#include "AbstractOTchannel.h"

void AbstractOTchannel::attachOTchannel(AbstractOTchannel &childOTtoAttach) {
  childOT = &childOTtoAttach;
  childOT->setParrentOTchannel(*this);
}

    
void AbstractOTchannel::setParrentOTchannel(AbstractOTchannel &parentOTchannelToAttach) {
  parentOT = &parentOTchannelToAttach;
}
