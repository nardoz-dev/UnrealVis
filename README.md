# UnrealVis
**Laboratory for Optimization Techniques in Scientific Visualization using Unreal Engine 5**

UnrealVis is a high-performance framework developed in **Unreal Engine 5.3** designed to visualize complex molecular structures (PDB files) and benchmark rendering optimization techniques in real-time.

Unlike standard visualization tools, UnrealVis includes a custom-built **Hardware & Performance Monitoring System** written in C++, allowing researchers to measure the impact of techniques like Nanite, Culling, and LODs on CPU, GPU, and RAM usage.

---

## 🚀 Key Features

* **🧬 PDB Importer:** Native parsing and visualization of Protein Data Bank (`.pdb`) files.
* **☁️ Automatic Metadata Fetching:** Integrated **RCSB PDB API** client. When a molecule is loaded, the system automatically fetches Title, Classification, and Organism data via HTTP REST calls.
* **📈 Scientific Charting System:** Custom C++ `AdvancedLineChart` widget featuring:
    * Auto-scaling axes.
    * Scientific grid layout.
    * Real-time plotting of FPS, Frame Time, and Memory.
* **💻 Hardware Monitoring:**
    * **CPU Load %** (via Windows PDH libraries).
    * **RAM Usage** (Native UE Platform Memory).
    * **GPU Frame Time** (via RenderCore/RHI).
* **💾 Data Export:** Ability to export simulation metrics (FPS, hardware stats over time) into **JSON** format for external analysis.

---

## 🛠️ Requirements & Dependencies

To edit or build the project, you need the following environment:

* **Unreal Engine Version:** 5.3.x
* **IDE:** Visual Studio 2022 (Community or Professional)
    * *Workload required:* "Game Development with C++"
* **Platform:** Windows 10/11 (Required for CPU PDH counters)

### Modules & Plugins
This project minimizes external plugin dependencies by using custom C++ modules.
**Core dependencies** configured in `UnrealVis.Build.cs`:
* `HTTP`, `Json`, `JsonUtilities` (For API & Export)
* `Slate`, `SlateCore`, `UMG` (For the UI & Charts)
* `RenderCore`, `RHI` (For GPU Stats)
* `Pdh` (Windows System Library for CPU monitoring)

External plugins : 
**External dependencies**: configured via Epic Games Launcher (download it via Fab.com): 
- BlueprintFile SDK


> **Note:** The legacy *Kantan Charts* plugin is **NO LONGER REQUIRED** as it has been replaced by the internal `UAdvancedLineChart` class.

---

## ⚙️ Installation & Setup

### 1. Clone the Repository
```bash
git clone [git@github.com:nardoz-dev/UnrealVis.git](git@github.com:nardoz-dev/UnrealVis.git)
git fetch --all
git checkout feat/integration5.3 (waiting for PR to dev)
cd UnrealVis
```

> **Note:** Ensure you have the right version of the engine installed, check over EpicGames Launcher: Unreal Engine Section! (5.3.2)

---
### 1. Standalone version
- [Download here](https://drive.google.com/file/d/1oxDBSXoJDe5PsnNAycQMpisXx_6LAYt-/view?usp=drive_link)
- You can download the application by following this link : ( in this case you don't need to install nothing ) 
- Extract the rar and run the .exe file!

# Author : 
| **Name and Surname** | **Linkedin** | **GitHub** |
| :---: | :---: | :---: |
| `Andrea Nardocci ` | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/LogoIn.png)](https://www.linkedin.com/in/andrea-nardocci) | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/GitHubLogo.png)](https://github.com/nardoz-dev) |
