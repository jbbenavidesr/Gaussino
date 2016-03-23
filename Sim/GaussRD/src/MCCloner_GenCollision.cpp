#include "MCCloner.h"

LHCb::GenCollision* MCCloner::getStoredGenCollision(const LHCb::GenCollision* gc) {
    auto result = m_gencollisions.find(gc);
    if (result == m_gencollisions.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::GenCollision* MCCloner::cloneKeyedGenCollision(const LHCb::GenCollision* gc) {
    auto clone = getStoredGenCollision(gc);
    if (!clone) {
        LHCb::GenCollision* clone = new LHCb::GenCollision();
        clone->setIsSignal(gc->isSignal());
        clone->setProcessType(gc->processType());
        clone->setSHat(gc->sHat());
        clone->setTHat(gc->tHat());
        clone->setUHat(gc->uHat());
        clone->setPtHat(gc->ptHat());
        clone->setX1Bjorken(gc->x1Bjorken());
        clone->setX2Bjorken(gc->x2Bjorken());
        // What to do with the HepMCEvent? Currently not cloned ...
        // clone->setEvent( theHepMCEvent );
        m_gencollisions.insert(std::pair<const LHCb::GenCollision*, LHCb::GenCollision*>(gc, clone));
        getClonedGenCollisions()->insert(clone);
    }
    return clone;
}

LHCb::GenCollision* MCCloner::cloneGenCollision(const LHCb::GenCollision* gc) {
    if (!gc) return NULL;
    LHCb::GenCollision* clone = getStoredGenCollision(gc);
    return (clone ? clone : this->doCloneGenCollision(gc));
}

LHCb::GenCollision* MCCloner::doCloneGenCollision(const LHCb::GenCollision* gc) {
    if (!gc) return NULL;

    // Clone the GenCollision
    LHCb::GenCollision* clone = cloneKeyedGenCollision(gc);

    return clone;
}

LHCb::GenCollisions* MCCloner::getClonedGenCollisions() {
    if (!m_list_gencollisions) {
        m_list_gencollisions = new LHCb::GenCollisions();
    }
    return m_list_gencollisions;
}
