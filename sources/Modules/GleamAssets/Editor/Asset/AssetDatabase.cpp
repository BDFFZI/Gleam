#include "AssetDatabase.h"

#include "AssetImporter.h"

namespace Gleam
{
    bool AssetDatabase::CanLoad(const std::filesystem::path& path)
    {
        return AssetImporter::HasImporter(path);
    }
}