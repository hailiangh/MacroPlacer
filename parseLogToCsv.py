import os

def main():
    dir = 'input/'
    ext = '.log'
    
    file = "input/job_16.log"
    parseLogToCsv(file)


def parseLogToCsv(file):
    print("Parse file [" + file + "]..")
    
    csvList = []
    csv = []

    obj = 0
    time = 0
    objValid = 0

    with open(file) as fp:
        for line in fp:
            # Clear csv buffer for a new job
            # if line.startswith("Run Job"):
            #     csv.clear()
            #     print("clear at run job")
    
            # We find the last solution and its next first runtime pair.
    
            # find a value for objective.
            # if another objective appears, it automatically updates.
            # if line.startswith("Found phase-1 solution: relaxation"):
            #     tokens = line.split(" ")
            #     obj = tokens[-1]
            #     objValid = 1
            
            if line.startswith("Found heuristic solution: objective"):
                tokens = line.split(" ")
                obj = tokens[-1]
                objValid = 1

            if line.startswith("Elapsed time for NoRel heuristic:") and objValid==1:
                tokens = line.split(" ")
                timeStr = tokens[5]
                time = timeStr[:-1]
                objValid = 0    
                csv.append(time+","+obj+"")
                #print("time ", time, " obj ", obj)

            if line.startswith("Best objective"):
                csvList.append(csv)
                print("append csv, len = ", len(csv))
                csv = []
                print("clear at best objective")

    fp.close()

    fileNameNoExt = os.path.splitext(file)[0]
    i = 0
    print("csvList.size = ", len(csvList))
    for c in csvList:
        print("c.size = ", len(c))
        i = i + 1
        fileOut = fileNameNoExt + "_" + str(i) + ".csv"
        
        with open(fileOut, "w") as fp:
            print("\tWrite to ", fileOut)
            for line in c:
                fp.write(line)
            fp.close()

    print("-----------------------------")

if __name__=="__main__":
    main()
    
            
                

            
                
        
            

        
    

    

