import argparse
import os, shutil
import filecmp

def create_hashes(sync_dir, showmap_path, program):
    if os.path.exists(os.path.join(sync_dir, 'hashes')):
        shutil.rmtree(os.path.join(sync_dir, 'hashes'))
    os.mkdir(os.path.join(sync_dir, 'hashes'))
    
    crashes = os.listdir(os.path.join(sync_dir, 'crashes'))
    if 'README.txt' in crashes:
        crashes.remove('README.txt')
        
    for crash in crashes:
        crash_path = os.path.join(sync_dir, 'crashes', crash)
        os.system(showmap_path + ' -o ' + os.path.join(sync_dir, 'hashes', crash) + ' -- ' + ' '.join([arg if arg != '@@' else crash_path for arg in program]))
    
def remove_duplicates(sync_dir_1, sync_dir_2):
    hashes_1 = os.listdir(os.path.join(sync_dir_1, 'hashes'))
    hashes_2 = os.listdir(os.path.join(sync_dir_2, 'hashes'))
    
    for h_1 in hashes_1:
        for h_2 in hashes_2:
            if filecmp.cmp(os.path.join(sync_dir_1, 'hashes', h_1), os.path.join(sync_dir_2, 'hashes', h_2), False):
                os.remove(os.path.join(sync_dir_1, 'hashes', h_1))
                break
    
def main():
    parser = argparse.ArgumentParser(description='Remove duplicates from parallel fuzzing.')
    
    parser.add_argument('--sync_dir', type=str, required=True, help='Path to sync dir.')
    parser.add_argument('--showmap_path', type=str, required=True, help='Path to \'afl-showmap\'')
    parser.add_argument('program', type=str, nargs='+', help='Program and arguments')
    
    args = parser.parse_args()
    
    fuzzer_dirs = os.listdir(args.sync_dir)
    
    for d in fuzzer_dirs:
        create_hashes(os.path.join(args.sync_dir, d), args.showmap_path, args.program)
    
    for i in range(len(fuzzer_dirs)):
        for j in range(i+1, len(fuzzer_dirs)):
            remove_duplicates(os.path.join(args.sync_dir, fuzzer_dirs[i]), os.path.join(args.sync_dir, fuzzer_dirs[j]))
    
    cnt = 0
    for d in fuzzer_dirs:
        cnt += len(os.listdir(os.path.join(args.sync_dir, d, 'hashes')))
    
    print(cnt)
    
import sys
    
if __name__ == '__main__':
    main()
