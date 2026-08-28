#include <dirent.h>
#include <fnmatch.h>

namespace juce
{
class DirectoryIterator::NativeIterator::Pimpl
{
public:
    Pimpl (const File& directory, const String& wildcardIn)
        : parentDir (File::addTrailingSeparator (directory.getFullPathName())),
          wildcard (wildcardIn),
          directoryHandle (opendir (directory.getFullPathName().toRawUTF8()))
    {
    }

    ~Pimpl()
    {
        if (directoryHandle != nullptr)
            closedir (directoryHandle);
    }

    bool next (String& filenameFound,
               bool* isDirectory, bool* isHidden, int64* fileSize,
               Time* modificationTime, Time* creationTime, bool* isReadOnly)
    {
        while (directoryHandle != nullptr)
        {
            const auto* entry = readdir (directoryHandle);

            if (entry == nullptr)
                return false;

            if (fnmatch (wildcard.toRawUTF8(), entry->d_name, 0) != 0)
                continue;

            filenameFound = CharPointer_UTF8 (entry->d_name);
            const auto fullPath = parentDir + filenameFound;
            struct stat info {};

            if (stat (fullPath.toRawUTF8(), &info) == 0)
            {
                if (isDirectory != nullptr)
                    *isDirectory = S_ISDIR (info.st_mode);

                if (fileSize != nullptr)
                    *fileSize = (int64) info.st_size;

                if (modificationTime != nullptr)
                    *modificationTime = Time ((int64) info.st_mtime * 1000);

                if (creationTime != nullptr)
                    *creationTime = Time ((int64) info.st_ctime * 1000);

                if (isReadOnly != nullptr)
                    *isReadOnly = access (fullPath.toRawUTF8(), W_OK) != 0;
            }

            if (isHidden != nullptr)
                *isHidden = filenameFound.startsWithChar ('.');

            return true;
        }

        return false;
    }

private:
    String parentDir, wildcard;
    DIR* directoryHandle = nullptr;

    JUCE_DECLARE_NON_COPYABLE (Pimpl)
};

DirectoryIterator::NativeIterator::NativeIterator (const File& directory, const String& wildcard)
    : pimpl (new Pimpl (directory, wildcard))
{
}

DirectoryIterator::NativeIterator::~NativeIterator() = default;

bool DirectoryIterator::NativeIterator::next (String& filenameFound,
                                              bool* isDirectory, bool* isHidden, int64* fileSize,
                                              Time* modificationTime, Time* creationTime, bool* isReadOnly)
{
    return pimpl->next (filenameFound, isDirectory, isHidden, fileSize,
                        modificationTime, creationTime, isReadOnly);
}
} // namespace juce
