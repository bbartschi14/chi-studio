#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace CHISTUDIO {

/** Classes can implement IKeyframeable when they have keyframe tracks and animateable data. */
class IKeyframeable
{
public:
	IKeyframeable(std::string InName) : SelectedTrackIndex(0), SelectedKeyframeableIndex(0), Name(InName) {}
	IKeyframeable() : SelectedTrackIndex(0), Name("") {}
	~IKeyframeable() {}

	// Update whatever properties this keyframeable allows to be keyframes, calculated using the current keyframes
	virtual void ApplyKeyframeData(int InFrame) {};

	virtual std::vector<std::string> GetKeyframeTrackNames() const { return std::vector<std::string>(); };

	virtual void CreateKeyframeOnTrack(std::string InTrackName, int InFrame) {};

	virtual void DeleteKeyframeOnTrack(std::string InTrackName, int InIndex) {};

	virtual std::vector<class IKeyframeBase*> GetKeyframesOnTrack(std::string InTrackName) { return std::vector<class IKeyframeBase*>(); };

	/** Allows parent classes to implement keyframeable, and still have keyframeable children to process. */
	virtual std::vector<IKeyframeable*> GetKeyframeables() { return std::vector<IKeyframeable*>(); }

	int SelectedTrackIndex;
	int SelectedKeyframeableIndex;
	std::string Name;
private:
};
}
