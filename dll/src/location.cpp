#include "location.h"

#include <filesystem>

static std::wstring ref{};
static std::wstring location{};

void setLocation(const std::wstring& location) {
	ref = location;
}

const std::wstring& getLocation() {
	if (!location.empty()) return location;
	
	if (std::filesystem::is_symlink(ref)) {
		location = std::filesystem::read_symlink(ref).parent_path();
	} else {
		location = std::filesystem::path(ref).parent_path();
	}

	return location;
}