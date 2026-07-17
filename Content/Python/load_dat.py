import unreal
import os

def process_turbulence_data(dat_path):
    # 1. Setup Paths
    raw_path = dat_path.replace('\\', '/')
    file_name = os.path.basename(raw_path).split('.')[0]
    csv_path = raw_path.replace(".dat", ".csv")
    
    # This must match the folder where you created your Struct in Step 4
    # Example: "/Game/Data/F_Turbulence_Row"
    struct_asset_path = "/Game/F_Turbulence_Row" 
    
    # 2. Convert .dat to CSV with the "Name" column
    try:
        with open(raw_path, 'r') as dat_file, open(csv_path, 'w') as csv_file:
            lines = dat_file.readlines()
            
        # Write Headers - Must match the order of your 14 variables in the Struct
            # The first column 'Name' is mandatory for Unreal DataTables
            headers = "Name,y,u/ub,u/utau,v/ub,w/ub,u2/utau,v2/utau,w2/utau,uv/utau,uw/utau,vw/utau,omx,psi\n"
            csv_file.write(headers)
            
            row_count = 0
            for line in lines:
                parts = line.split()
                # Skip header text by checking if the first part is a number
                if parts and (parts[0][0].isdigit() or parts[0][0] == '-'):
                    # We add 'row_count' as the unique 'Name' for this row
                    csv_file.write(f"Row_{row_count}," + ",".join(parts) + "\n")
                    row_count += 1
        
        # 3. Import the CSV as a DataTable
        import_as_datatable(csv_path, file_name, struct_asset_path)
        
    except Exception as e:
        unreal.log_error(f"Failed to process data: {str(e)}")

def import_as_datatable(csv_path, asset_name, struct_path):
    # Load the struct you created in Step 4
    row_struct = unreal.load_asset(struct_path)
    if not row_struct:
        unreal.log_error(f"Could not find Struct at {struct_path}. Please check the path!")
        return

    task = unreal.AssetImportTask()
    task.filename = csv_path
    task.destination_path = "/Game/TurbulenceData"
    task.destination_name = asset_name
    task.automated = True
    task.replace_existing = True
    
    factory = unreal.CSVImportFactory()
    factory.automated_import_settings.import_row_struct = row_struct
    task.factory = factory
    
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    unreal.log(f"Successfully created DataTable: {asset_name} using {struct_path}")