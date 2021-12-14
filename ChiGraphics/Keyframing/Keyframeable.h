#pragma once
#include <vector>
#include <memory>
#include <string>

namespace CHISTUDIO {

//
class IKeyframeable
{
public:
	IKeyframeable() : SelectedTrackIndex(0) {}
	~IKeyframeable() {}
	// Update whatever properties this keyframeable allows to be keyframes, calculated using the current keyframes
	virtual void ApplyKeyframeData(int InFrame) = 0;
	virtual std::vector<std::string> GetKeyframeTrackNames() const = 0;
	virtual void CreateKeyframeOnTrack(std::string InTrackName, int InFrame) = 0;
	virtual std::vector<class IKeyframeBase*> GetKeyframesOnTrack(std::string InTrackName) = 0;

	int SelectedTrackIndex;
private:
};

}
