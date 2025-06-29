#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <iostream>
#include <iomanip>

using json = nlohmann::json;

struct AddCont {
	std::vector<std::string> serviceIdForSharing;
};

struct Amm {
	int pagetableMemorySizeInMib;
	int vaRangeInGib;
};

struct GameIntent {
	struct PermittedIntent {
		std::string intentType;
	};
	std::vector<PermittedIntent> permittedIntents;
};

struct Kernel {
	int cpuPageTableSize;
	int flexibleMemorySize;
	int gpuPageTableSize;
};

struct LocalizedParameters {
	struct LanguageEntry {
		std::string titleName;
	};
	std::unordered_map<std::string, LanguageEntry> languages;
	std::string defaultLanguage;
};

struct PubTools {
	std::string creationDate;
	std::string loudnessSnd0;
	bool submission;
	std::string toolVersion;
};

struct AgeLevel {
	std::unordered_map<std::string, int> countryRatings;
	int defaultRating;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AddCont, serviceIdForSharing)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Amm, pagetableMemorySizeInMib, vaRangeInGib)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameIntent::PermittedIntent, intentType)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameIntent, permittedIntents)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Kernel, cpuPageTableSize, flexibleMemorySize, gpuPageTableSize)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LocalizedParameters::LanguageEntry, titleName)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PubTools, creationDate, loudnessSnd0, submission, toolVersion)

void to_json(json& j, const AgeLevel& a) {
	j = a.countryRatings;
	j["default"] = a.defaultRating;
}

void from_json(const json& j, AgeLevel& a) {
	a.defaultRating = j["default"];
	j.get_to(a.countryRatings);
	a.countryRatings.erase("default");
}

void to_json(json& j, const LocalizedParameters& lp) {
	j = lp.languages;
	j["defaultLanguage"] = lp.defaultLanguage;
}

void from_json(const json& j, LocalizedParameters& lp) {
	lp.defaultLanguage = j["defaultLanguage"];
	for (auto& [key, value] : j.items()) {
		if (key != "defaultLanguage") {
			lp.languages[key] = value.get<LocalizedParameters::LanguageEntry>();
		}
	}
}

struct ParamJson {
	AddCont addcont;
	AgeLevel ageLevel;
	Amm amm;
	int applicationCategoryType;
	std::string applicationDrmType;
	int attribute;
	int attribute2;
	int attribute3;
	std::string conceptId;
	int contentBadgeType;
	std::string contentId;
	std::string contentVersion;
	int downloadDataSize;
	GameIntent gameIntent;
	Kernel kernel;
	LocalizedParameters localizedParameters;
	std::string masterVersion;
	std::string originContentVersion;
	PubTools pubtools;
	std::string requiredSystemSoftwareVersion;
	std::string sdkVersion;
	std::string targetContentVersion;
	std::string titleId;
	int userDefinedParam1;
	int userDefinedParam2;
	int userDefinedParam3;
	int userDefinedParam4;
	std::string versionFileUri;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ParamJson,
	addcont,
	ageLevel,
	amm,
	applicationCategoryType,
	applicationDrmType,
	attribute,
	attribute2,
	attribute3,
	conceptId,
	contentBadgeType,
	contentId,
	contentVersion,
	downloadDataSize,
	gameIntent,
	kernel,
	localizedParameters,
	masterVersion,
	originContentVersion,
	pubtools,
	requiredSystemSoftwareVersion,
	sdkVersion,
	targetContentVersion,
	titleId,
	userDefinedParam1,
	userDefinedParam2,
	userDefinedParam3,
	userDefinedParam4,
	versionFileUri
)

void printAddCont(const AddCont& addcont) {
	std::cout << "AddCont:\n";
	std::cout << "  serviceIdForSharing:\n";
	for (const auto& id : addcont.serviceIdForSharing) {
		std::cout << "    - \"" << id << "\"\n";
	}
}

void printAmm(const Amm& amm) {
	std::cout << "Amm:\n";
	std::cout << "  pagetableMemorySizeInMib: " << amm.pagetableMemorySizeInMib << "\n";
	std::cout << "  vaRangeInGib: " << amm.vaRangeInGib << "\n";
}

void printGameIntent(const GameIntent& gameIntent) {
	std::cout << "GameIntent:\n";
	std::cout << "  permittedIntents:\n";
	for (const auto& intent : gameIntent.permittedIntents) {
		std::cout << "    - intentType: \"" << intent.intentType << "\"\n";
	}
}

void printKernel(const Kernel& kernel) {
	std::cout << "Kernel:\n";
	std::cout << "  cpuPageTableSize: " << kernel.cpuPageTableSize << "\n";
	std::cout << "  flexibleMemorySize: " << kernel.flexibleMemorySize << "\n";
	std::cout << "  gpuPageTableSize: " << kernel.gpuPageTableSize << "\n";
}

void printLocalizedParameters(const LocalizedParameters& localizedParameters) {
	std::cout << "LocalizedParameters:\n";
	std::cout << "  defaultLanguage: \"" << localizedParameters.defaultLanguage << "\"\n";
	std::cout << "  languages:\n";
	for (const auto& [lang, entry] : localizedParameters.languages) {
		std::cout << "    " << std::left << std::setw(10) << lang
			<< ": \"" << entry.titleName << "\"\n";
	}
}

void printPubTools(const PubTools& pubtools) {
	std::cout << "PubTools:\n";
	std::cout << "  creationDate: \"" << pubtools.creationDate << "\"\n";
	std::cout << "  loudnessSnd0: \"" << pubtools.loudnessSnd0 << "\"\n";
	std::cout << "  submission: " << std::boolalpha << pubtools.submission << "\n";
	std::cout << "  toolVersion: \"" << pubtools.toolVersion << "\"\n";
}

void printAgeLevel(const AgeLevel& ageLevel) {
	std::cout << "AgeLevel:\n";
	std::cout << "  default: " << ageLevel.defaultRating << "\n";
	std::cout << "  countryRatings:\n";
	for (const auto& [country, rating] : ageLevel.countryRatings) {
		std::cout << "    " << std::left << std::setw(4) << country
			<< ": " << rating << "\n";
	}
}

void printParamJson(const ParamJson& params) {
	std::cout << "========================================\n";
	std::cout << "          PARAM.JSON CONTENTS           \n";
	std::cout << "========================================\n\n";

	// Basic info
	std::cout << "Basic Information:\n";
	std::cout << "  contentId: \"" << params.contentId << "\"\n";
	std::cout << "  titleId: \"" << params.titleId << "\"\n";
	std::cout << "  conceptId: \"" << params.conceptId << "\"\n";
	std::cout << "  contentVersion: \"" << params.contentVersion << "\"\n";
	std::cout << "  masterVersion: \"" << params.masterVersion << "\"\n";
	std::cout << "  originContentVersion: \"" << params.originContentVersion << "\"\n";
	std::cout << "  targetContentVersion: \"" << params.targetContentVersion << "\"\n";
	std::cout << "  requiredSystemSoftwareVersion: \"" << params.requiredSystemSoftwareVersion << "\"\n";
	std::cout << "  sdkVersion: \"" << params.sdkVersion << "\"\n";
	std::cout << "  versionFileUri: \"" << params.versionFileUri << "\"\n\n";

	// Application info
	std::cout << "Application Info:\n";
	std::cout << "  applicationCategoryType: " << params.applicationCategoryType << "\n";
	std::cout << "  applicationDrmType: \"" << params.applicationDrmType << "\"\n";
	std::cout << "  contentBadgeType: " << params.contentBadgeType << "\n";
	std::cout << "  downloadDataSize: " << params.downloadDataSize << "\n\n";

	// Attributes
	std::cout << "Attributes:\n";
	std::cout << "  attribute: " << params.attribute << "\n";
	std::cout << "  attribute2: " << params.attribute2 << "\n";
	std::cout << "  attribute3: " << params.attribute3 << "\n\n";

	// User defined params
	std::cout << "User Defined Parameters:\n";
	std::cout << "  userDefinedParam1: " << params.userDefinedParam1 << "\n";
	std::cout << "  userDefinedParam2: " << params.userDefinedParam2 << "\n";
	std::cout << "  userDefinedParam3: " << params.userDefinedParam3 << "\n";
	std::cout << "  userDefinedParam4: " << params.userDefinedParam4 << "\n\n";

	// Print nested structures
	printAddCont(params.addcont);
	std::cout << "\n";
	printAgeLevel(params.ageLevel);
	std::cout << "\n";
	printAmm(params.amm);
	std::cout << "\n";
	printGameIntent(params.gameIntent);
	std::cout << "\n";
	printKernel(params.kernel);
	std::cout << "\n";
	printLocalizedParameters(params.localizedParameters);
	std::cout << "\n";
	printPubTools(params.pubtools);
}

int main() {
	try {
		// Read JSON file
		std::ifstream file("param.json");
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open param.json");
		}

		// Parse JSON
		json j = json::parse(file);
		ParamJson params = j.get<ParamJson>();

		// Print all contents
		printParamJson(params);

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}