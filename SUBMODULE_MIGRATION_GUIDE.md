# IEEE Standards Submodule Migration Guide

## ✅ Repositories Created Successfully

The following GitHub repositories have been created and are ready to receive code:

1. **IEEE_802.1AS_2021** - https://github.com/zarfld/IEEE_802.1AS_2021
2. **IEEE_802.1Q_2020** - https://github.com/zarfld/IEEE_802.1Q_2020
3. **IEEE_802.1BA_2016** - https://github.com/zarfld/IEEE_802.1BA_2016
4. **IEEE_1588_2019** - https://github.com/zarfld/IEEE_1588_2019
5. **IEEE_1722_2016** - https://github.com/zarfld/IEEE_1722_2016
6. **IEEE_1722.1_2021** - https://github.com/zarfld/IEEE_1722.1_2021
7. **IEEE_1722.1_2013** - https://github.com/zarfld/IEEE_1722.1_2013

## 📋 Migration Steps

### Step 1: Push Existing Code to New Repositories

#### For IEEE 802.1AS-2021
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/802.1AS/2021

# Initialize git (if not already initialized)
git init

# Add all files
git add .
git commit -m "Initial commit: IEEE 802.1AS-2021 gPTP implementation from libmedia-network-standards"

# Link to new repository
git remote add origin https://github.com/zarfld/IEEE_802.1AS_2021.git
git branch -M main

# Push to GitHub
git push -u origin main
```

#### For IEEE 1722-2016
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/1722/2016

git init
git add .
git commit -m "Initial commit: IEEE 1722-2016 AVTP implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_1722_2016.git
git branch -M main
git push -u origin main
```

#### For IEEE 1722.1-2021
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/1722.1/2021

git init
git add .
git commit -m "Initial commit: IEEE 1722.1-2021 AVDECC implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_1722.1_2021.git
git branch -M main
git push -u origin main
```

#### For IEEE 1722.1-2013
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/1722.1/2013

git init
git add .
git commit -m "Initial commit: IEEE 1722.1-2013 AVDECC legacy implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_1722.1_2013.git
git branch -M main
git push -u origin main
```

#### For IEEE 1588-2019
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/1588/PTP/2019

git init
git add .
git commit -m "Initial commit: IEEE 1588-2019 PTPv2 implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_1588_2019.git
git branch -M main
git push -u origin main
```

#### For IEEE 802.1Q-2020
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/802.1/Q/2020

git init
git add .
git commit -m "Initial commit: IEEE 802.1Q-2020 VLAN/QoS implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_802.1Q_2020.git
git branch -M main
git push -u origin main
```

#### For IEEE 802.1BA-2016
```powershell
cd d:/Repos/libmedia-network-standards/IEEE/802.1/BA/2016

git init
git add .
git commit -m "Initial commit: IEEE 802.1BA-2016 AVB Systems implementation from libmedia-network-standards"
git remote add origin https://github.com/zarfld/IEEE_802.1BA_2016.git
git branch -M main
git push -u origin main
```

### Step 2: Add Submodules to Main Repository

After pushing code to the new repositories, add them as submodules to the main repository:

```powershell
cd d:/Repos/libmedia-network-standards

# Remove old folder structure (IEEE/802.1AS will be replaced by submodule at IEEE/802.1/AS/2021)
git rm -r IEEE/802.1AS

# Add IEEE 802.1AS-2021 submodule to CORRECT location
git submodule add https://github.com/zarfld/IEEE_802.1AS_2021.git IEEE/802.1/AS/2021

# Add IEEE 802.1Q-2020 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_802.1Q_2020.git IEEE/802.1/Q/2020

# Add IEEE 802.1BA-2016 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_802.1BA_2016.git IEEE/802.1/BA/2016

# Add IEEE 1588-2019 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_1588_2019.git IEEE/1588/PTP/2019

# Add IEEE 1722-2016 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_1722_2016.git IEEE/1722/2016

# Add IEEE 1722.1-2021 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_1722.1_2021.git IEEE/1722.1/2021

# Add IEEE 1722.1-2013 submodule (already in correct location)
git submodule add https://github.com/zarfld/IEEE_1722.1_2013.git IEEE/1722.1/2013

# Initialize and update submodules
git submodule update --init --recursive

# Commit submodule configuration
git add .gitmodules IEEE/
git commit -m "Add IEEE standards as submodules"
git push
```

### Step 3: Update Build System

The main repository CMakeLists.txt should now reference submodules:

```cmake
# Main repository CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(libmedia_network_standards)

# IEEE Standards Submodules
add_subdirectory(IEEE/802.1/AS/2021)    # IEEE 802.1AS-2021 gPTP
add_subdirectory(IEEE/802.1/Q/2020)     # IEEE 802.1Q-2020 VLAN/QoS
add_subdirectory(IEEE/802.1/BA/2016)    # IEEE 802.1BA-2016 AVB Systems
add_subdirectory(IEEE/1588/PTP/2019)    # IEEE 1588-2019 PTPv2
add_subdirectory(IEEE/1722/2016)        # IEEE 1722-2016 AVTP
add_subdirectory(IEEE/1722.1/2021)      # IEEE 1722.1-2021 AVDECC
add_subdirectory(IEEE/1722.1/2013)      # IEEE 1722.1-2013 AVDECC Legacy

# Cross-standard integration
add_library(standards_integration INTERFACE)
target_link_libraries(standards_integration INTERFACE
    ieee_802_1as_2021_core
    ieee_802_1q_2020_core
    ieee_802_1ba_2016_core
    ieee_1588_2019_core
    ieee_1722_2016_core
    ieee_1722_1_2021_core
    ieee_1722_1_2013_core
)
```

## 🎯 Benefits Achieved

### ✅ Clean Separation
- Each IEEE standard has its own independent repository
- Clear version boundaries (2021, 2016, 2013)
- Independent git history per standard

### ✅ Correct Folder Structure
- IEEE 802.1AS-2021 now in `IEEE/802.1/AS/2021` (correct)
- No more `IEEE/802.1AS` (incorrect) folder
- Follows IEEE standard naming exactly

### ✅ Standards Compliance
- Repository names follow IEEE naming: `IEEE_802.1AS_2021`
- Each standard version independently versioned
- Clear boundaries prevent cross-contamination

### ✅ Development Benefits
- Parallel development on different standards
- Standard-specific CI/CD pipelines
- Reduced repository complexity
- Easier collaboration per standard

## 🔄 Developer Workflow with Submodules

### Cloning Repository with Submodules
```powershell
# Clone main repository with all submodules
git clone --recursive https://github.com/zarfld/libmedia-network-standards.git

# Or if already cloned without submodules
git submodule update --init --recursive
```

### Updating Submodule Code
```powershell
# Work in submodule directory
cd IEEE/802.1/AS/2021

# Make changes
git add .
git commit -m "Fix: IEEE 802.1AS-2021 path delay calculation"
git push

# Update main repository to reference new submodule commit
cd ../../../..
git add IEEE/802.1/AS/2021
git commit -m "Update IEEE 802.1AS-2021 submodule"
git push
```

### Updating All Submodules
```powershell
# Pull latest changes for all submodules
git submodule update --remote --merge
```

## 📚 Next Steps

1. ✅ **Repositories Created** - Done!
2. ⏳ **Push Code to Repositories** - Execute Step 1 commands above
3. ⏳ **Add Submodules** - Execute Step 2 commands above
4. ⏳ **Update Build System** - Execute Step 3 configuration
5. ⏳ **Test Build** - Verify everything builds correctly
6. ⏳ **Update Documentation** - Document submodule workflow for team

## 🎯 Repository Mapping

| Source Location (Old) | Repository | Submodule Location (New) |
|-----------------------|------------|---------------------------|
| `IEEE/802.1AS/2021` | `IEEE_802.1AS_2021` | `IEEE/802.1/AS/2021` |
| `IEEE/802.1/Q/2020` | `IEEE_802.1Q_2020` | `IEEE/802.1/Q/2020` |
| `IEEE/802.1/BA/2016` | `IEEE_802.1BA_2016` | `IEEE/802.1/BA/2016` |
| `IEEE/1588/PTP/2019` | `IEEE_1588_2019` | `IEEE/1588/PTP/2019` |
| `IEEE/1722/2016` | `IEEE_1722_2016` | `IEEE/1722/2016` |
| `IEEE/1722.1/2021` | `IEEE_1722.1_2021` | `IEEE/1722.1/2021` |
| `IEEE/1722.1/2013` | `IEEE_1722.1_2013` | `IEEE/1722.1/2013` |

## ⚠️ Important Notes

- **Backup first**: Ensure all changes are committed before migration
- **Clean working directory**: No uncommitted changes in source folders
- **Test after migration**: Verify build system works with submodules
- **Team communication**: Notify team members about new submodule workflow
