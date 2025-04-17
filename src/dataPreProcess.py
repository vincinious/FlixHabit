import pandas as pd

file_path = "data/netflix_users.csv"
data = pd.read_csv(file_path)

# print(data)

"""
       User_ID              Name  Age    Country Subscription_Type  Watch_Time_Hours Favorite_Genre  Last_Login
0            1    James Martinez   18     France           Premium             80.26          Drama  2024-05-12
1            2       John Miller   23        USA           Premium            321.75         Sci-Fi  2025-02-05
2            3        Emma Davis   60         UK             Basic             35.89         Comedy  2025-01-24
3            4       Emma Miller   44        USA           Premium            261.56    Documentary  2024-03-25
4            5        Jane Smith   68        USA          Standard            909.30          Drama  2025-01-14
...        ...               ...  ...        ...               ...               ...            ...         ...
24995    24996      David Miller   18  Australia           Premium            183.88         Horror  2025-01-18
24996    24997       Jane Miller   17        USA           Premium            112.37          Drama  2024-06-29
24997    24998      Sarah Miller   14        USA           Premium            351.80         Action  2024-10-16
24998    24999  Michael Williams   71        USA             Basic            655.89         Action  2024-08-27
24999    25000       James Jones   19    Germany           Premium            207.06    Documentary  2024-04-12

[25000 rows x 8 columns]
"""