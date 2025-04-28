import pandas as pd
df = pd.read_csv("trail_data.csv")
# Example: Load your dataset
# df = pd.read_csv('your_file.csv')  
# For now, let's assume df already exists and has 18 columns.

def average_blocks(df, block_size=49):
    num_rows = len(df)
    num_blocks = num_rows // block_size  # Ignore leftovers if not divisible
    averaged_data = []

    for i in range(num_blocks):
        start_idx = i * block_size
        end_idx = start_idx + block_size
        block = df.iloc[start_idx:end_idx]
        block_mean = block.mean()
        averaged_data.append(block_mean)

    averaged_df = pd.DataFrame(averaged_data)
    return averaged_df


def average_blocks_filter(df, block_size=49, threshold=20):
    num_rows = len(df)
    num_blocks = num_rows // block_size  # Ignore leftovers if not divisible
    averaged_data = []

    for i in range(num_blocks):
        start_idx = i * block_size
        end_idx = start_idx + block_size
        block = df.iloc[start_idx:end_idx]
        block_mean = block.mean()

        # Only keep this block if all averages are greater than the threshold
        if (block_mean > threshold).all():
            averaged_data.append(block_mean)

    averaged_df = pd.DataFrame(averaged_data)  # Assign averaged data to DataFrame
    return averaged_df  # Return the DataFrame

averaged_df_blocks = average_blocks_filter(df)
print(averaged_df_blocks)

averaged_df = average_blocks(df)
print(averaged_df)

averaged_df.to_csv('problematic_trail_data_2.csv', index = False, header=False, mode ="w")