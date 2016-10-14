#ifndef GaussRedecayRetrieveFromService_H
#define GaussRedecayRetrieveFromService_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRedecayStr;  ///< GaussRedecay counter service
class IGaussRedecayCtr;  ///< GaussRedecay counter service

/** @class GaussRedecayRetrieveFromService GaussRedecayRetrieveFromService.h
 *
 *  An algorithm to control processing in a GaudiSequencer.
 *  Following members of the sequence are only processed
 *  if a new event needs to be generated
 *
 *  @author Dominik Muller
 *  @date   2016-3-21
 */
class GaussRedecayRetrieveFromService : public GaudiAlgorithm {
public:
    /// Standard constructor
    GaussRedecayRetrieveFromService(const std::string& Name,
                                    ISvcLocator* SvcLoc);

    virtual ~GaussRedecayRetrieveFromService();  ///< Destructor

    virtual StatusCode initialize();  ///< Algorithm initialization
    virtual StatusCode execute();     ///< Algorithm execution

protected:
    /** accessor to GaussRedecay Service
     *  @return pointer to GaussRedecay Service
     */
    inline IGaussRedecayStr* gaussRDStrSvc() const { return m_gaussRDStrSvc; }
    inline IGaussRedecayCtr* gaussRDCtrSvc() const { return m_gaussRDCtrSvc; }

private:
    std::string m_gaussRDSvcName;
    IGaussRedecayStr* m_gaussRDStrSvc;
    IGaussRedecayCtr* m_gaussRDCtrSvc;

    std::string
        m_particlesLocation;         ///< Location in TES of output MCParticles.
    std::string m_verticesLocation;  ///< Location in TES of output MCVertices.
    std::vector<std::string>
        m_hitsLocations;  ///< Location in TES of output MCHits.
    std::vector<std::string>
        m_calohitsLocations;         ///< Location in TES of output MCCaloHits.
    std::string m_richHitsLocation;  ///< Location in TES of output MCRichHits.
    std::string m_richOpticalPhotonsLocation;  ///< Location in TES of output
                                               /// MCRichOpticalPhotons.
    std::string
        m_richSegmentsLocation;  ///< Location in TES of output MCRichSegments.
    std::string
        m_richTracksLocation;  ///< Location in TES of output MCRichTracks.
    std::string
        m_GenCollisionsLocation;  ///< Location in TES of output GenCollisions.
    std::string m_mcHeader;       ///< Location in TES of MCHeader.

    template <typename T>
    StatusCode test_print_put(T* container, std::string loc);
};

template <typename T>
StatusCode GaussRedecayRetrieveFromService::test_print_put(T* container,
                                                           std::string loc) {
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << container->size() << " to "
                << loc << endmsg;
    }
    if (exist<T>(loc)) {
        auto previous = get<T>(loc);
        error() << "Location " << loc << " not empty. Found "
                << previous->size() << " objects!" << endmsg;
        return StatusCode::FAILURE;
    }
    put(container, loc);
    return StatusCode::SUCCESS;
}

#endif  // GaussRedecayRetrieveFromService_H
