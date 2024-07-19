#include "location.h"
#include <filesystem>

namespace fs = std::filesystem;

static fs::path ref{};
static fs::path location{};

void setLocation(fs::path location) {
	ref = location;
}

fs::path getLocation() {
	if (!location.empty()) return location;
	
	if (fs::is_symlink(ref)) {
		location = fs::read_symlink(ref);
	} else {
		location = ref;
	}

	return location;
}