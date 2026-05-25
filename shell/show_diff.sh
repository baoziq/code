read -p "input dir1 name: " dir1
read -p "input dir2 name: " dir2

echo "------------------------------ats-------------------------------------------"
icdiff -r $dir1 $dir2 --line-numbers --highlight

CCS_path1="$dir1/CCS/nginx/$dir1-l2.conf"
SNS_path1="$dir1/SNS/nginx/$dir1.conf"

CCS_path2="$dir2/CCS/nginx/$dir2-l2.conf"
SNS_path2="$dir2/SNS/nginx/$dir2.conf"

echo "------------------------------CCS-------------------------------------------"
icdiff $CCS_path2 $CCS_path1 --line-numbers --highlight
echo "------------------------------SNS-------------------------------------------"
icdiff $SNS_path2 $SNS_path1 --line-numbers --highlight
