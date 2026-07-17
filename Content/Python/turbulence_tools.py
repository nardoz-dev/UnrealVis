# # import unreal
# # import os
# # import tkinter as tk
# # from tkinter import filedialog

# # def open_windows_picker():
# #     # This opens a REAL Windows window that won't hide .dat files
# #     root = tk.Tk()
# #     root.withdraw() # Hide the tiny empty tkinter window
# #     root.attributes('-topmost', True) # Bring the file picker to the front
    
# #     # Select the file
# #     file_path = filedialog.askopenfilename(
# #         title="Select Turbulence .dat File",
# #         filetypes=[("DAT files", "*.dat"), ("All files", "*.*")]
# #     )
    
# #     root.destroy() # Close the background process

# #     if file_path:
# #         unreal.log(f"File Selected: {file_path}")
# #         process_dat_to_csv(file_path)
# #     else:
# #         unreal.log_warning("No file selected.")

# # def process_dat_to_csv(dat_path):
# #     # Standardize path
# #     dat_path = dat_path.replace('\\', '/')
# #     asset_name = os.path.basename(dat_path).split('.')[0]
# #     csv_path = dat_path.replace(".dat", ".csv")
    
# #     try:
# #         with open(dat_path, 'r') as f:
# #             lines = f.readlines()
            
# #         # The file has headers like "TITLE", "VARIABLES", "ZONE"
# #         # We need to find where the actual numbers start
# #         with open(csv_path, 'w') as out:
# #             # 14 Variables from plotyz_Retau150.dat
# #             out.write("Name,y,u/ub,u/utau,v/ub,w/ub,u2/utau,v2/utau,w2/utau,uv/utau,uw/utau,vw/utau,omx,psi\n")
            
# #             row_idx = 0
# #             for line in lines:
# #                 parts = line.split()
# #                 # If the line starts with a number or a minus sign, it's our data
# #                 if parts and (parts[0][0].isdigit() or parts[0][0] == '-'):
# #                     out.write(f"Row_{row_idx}," + ",".join(parts) + "\n")
# #                     row_idx += 1
        
# #         unreal.log(f"Converted {row_idx} rows to CSV. Now importing...")
# #         import_into_unreal(csv_path, asset_name)
        
# #     except Exception as e:
# #         unreal.log_error(f"Error: {str(e)}")

# # def import_into_unreal(csv_path, asset_name):
# #     task = unreal.AssetImportTask()
# #     task.filename = csv_path
# #     task.destination_path = "/Game/TurbulenceData"
# #     task.destination_name = asset_name
# #     task.automated = True
# #     task.replace_existing = True
    
# #     factory = unreal.CSVImportFactory()
# #     # Ensure you use the exact name of the Struct you created in Step 4
# #     factory.automated_import_settings.import_row_struct = unreal.load_asset("/Game/UI/F_Turbulence_Row")
# #     task.factory = factory
    
# #     unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

# import unreal
# import os
# import tkinter as tk
# from tkinter import filedialog

# def open_windows_picker():
#     root = tk.Tk()
#     root.withdraw()
#     root.attributes('-topmost', True)
#     file_path = filedialog.askopenfilename(
#         title="Select Turbulence .dat File",
#         filetypes=[("DAT files", "*.dat"), ("All files", "*.*")]
#     )
#     root.destroy()
#     if file_path:
#         process_dat_to_csv(file_path)

# def process_dat_to_csv(dat_path):
#     asset_name = os.path.basename(dat_path).split('.')[0]
#     csv_path = dat_path.replace(".dat", ".csv")
    
#     # Mapping for your two specific files
#     headers_plot = "Name,z,y,u_ub,u_utau,v_ub,w_ub,u2,v2,w2,uv,uw,vw,omx,psi\n"
#     headers_budget = "Name,z,y,var3,var4,var5,var6,var7,var8,var9,var10,var11,var12,var13,var14,var15,var16,var17,var18,var19\n"

#     try:
#         with open(dat_path, 'r') as f:
#             lines = f.readlines()

#         with open(csv_path, 'w') as out:
#             row_idx = 0
#             is_budget_file = False
            
#             for line in lines:
#                 parts = line.split()
#                 # Skip text-based headers
#                 if not parts or not (parts[0][0].isdigit() or parts[0][0] == '-'):
#                     continue
                
#                 # Check file type on the first data row
#                 if row_idx == 0:
#                     if len(parts) > 15: # Budget file has 19 columns
#                         out.write(headers_budget)
#                         is_budget_file = True
#                     else:
#                         out.write(headers_plot)

#                 # Write data row with mandatory 'Name' column
#                 out.write(f"Row_{row_idx}," + ",".join(parts) + "\n")
#                 row_idx += 1
        
#         # Choose the correct Struct based on detected file type
#         struct_name = "F_Turbulence_Row" if is_budget_file else "F_Turbulence_Row"
#         import_into_unreal(csv_path, asset_name, f"/Game/UI/{struct_name}")
        
#     except Exception as e:
#         unreal.log_error(f"Conversion Error: {str(e)}")

# def import_into_unreal(csv_path, asset_name, struct_path):
#     task = unreal.AssetImportTask()
#     task.filename = csv_path
#     task.destination_path = "/Game/TurbulenceData"
#     task.destination_name = asset_name
#     task.automated = True
#     task.replace_existing = True
    
#     factory = unreal.CSVImportFactory()
#     factory.automated_import_settings.import_row_struct = unreal.load_asset(struct_path)
#     task.factory = factory
    
#     unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
#     unreal.log(f"Successfully Imported: {asset_name}")


import unreal
import os
import tkinter as tk
from tkinter import filedialog

def open_windows_picker():
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    file_path = filedialog.askopenfilename(
        title="Select Turbulence .dat File",
        filetypes=[("DAT files", "*.dat"), ("All files", "*.*")]
    )
    root.destroy()
    if file_path:
        process_dat_to_csv(file_path)

def process_dat_to_csv(dat_path):
    asset_name = os.path.basename(dat_path).split('.')[0]
    csv_path = dat_path.replace(".dat", ".csv")
    
    # Mapping for your two specific files
    headers_plot = "Name,z,y,u_ub,u_utau,v_ub,w_ub,u2,v2,w2,uv,uw,vw,omx,psi\n"
    headers_budget = "Name,z,y,var3,var4,var5,var6,var7,var8,var9,var10,var11,var12,var13,var14,var15,var16,var17,var18,var19\n"

    try:
        with open(dat_path, 'r') as f:
            lines = f.readlines()

        with open(csv_path, 'w') as out:
            row_idx = 0
            is_budget_file = False
            
            for line in lines:
                parts = line.split()
                # Skip text-based headers
                if not parts or not (parts[0][0].isdigit() or parts[0][0] == '-'):
                    continue
                
                # Check file type on the first data row
                if row_idx == 0:
                    if len(parts) > 15: # Budget file has 19 data columns
                        out.write(headers_budget)
                        is_budget_file = True
                    else:
                        out.write(headers_plot)

                # Write data row with mandatory 'Name' column
                out.write(f"Row_{row_idx}," + ",".join(parts) + "\n")
                row_idx += 1
        
        # [FIXED] Choose the correct Struct based on detected file type.
        # Make sure "F_Budget_Row" exists in your project at /Game/UI/
        struct_name = "F_Budget_Row" if is_budget_file else "F_Turbulence_Row"
        import_into_unreal(csv_path, asset_name, f"/Game/UI/{struct_name}")
        
    except Exception as e:
        unreal.log_error(f"Conversion Error: {str(e)}")

def import_into_unreal(csv_path, asset_name, struct_path):
    task = unreal.AssetImportTask()
    task.filename = csv_path
    task.destination_path = "/Game/TurbulenceData"
    task.destination_name = asset_name
    task.automated = True
    task.replace_existing = True
    
    factory = unreal.CSVImportFactory()
    
    # [FIXED] Explicitly tell Unreal to import this as a Data Table
    factory.automated_import_settings.import_type = unreal.CSVImportType.ECSV_DATA_TABLE
    factory.automated_import_settings.import_row_struct = unreal.load_asset(struct_path)
    
    task.factory = factory
    
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    unreal.log(f"Successfully Imported: {asset_name}")

# Optional: Run it immediately if executing the script
# open_windows_picker()