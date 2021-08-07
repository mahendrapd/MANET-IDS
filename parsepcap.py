import csv
ColRange = 17
nFeatures = 21
nAttack = 5
nNode = 25
def csvread():
    global List
    pf = open("file24")
    List = pf.readlines()
    element = len(List)
    print (element)
    global a
    a = [[-1 for j in range(ColRange)] for i in range(len(List))]
    with open('file24', 'r') as csvFile:
        reader = csv.reader(csvFile)
        i = 0
        for row in reader:
            for j in range(ColRange):
                a[i][j] = row[j]
            i = i + 1
    fData = [[0 for j in range(nFeatures)] for i in range(element)]
    nNeibors = [0 for r in range(nNode)]
    nNData = [0 for r in range(nNode)]
    k = 0
    t = 0
    broad = ['10.0.1.255']
    failed = ['Route Error']
    nfailed = 0
    totalMessage = 0
    tHop = 0
    nHop = 0
    #attacklist = ['10.0.1.2' , '10.0.1.3' , '10.0.1.4' , '10.0.1.5' , '10.0.1.6' , '10.1.2.1' , '10.1.2.2' , '10.1.2.3' , '10.1.2.4' , '10.1.2.5']
    attacklist = ['10.1.2.8' , '10.1.2.14' , '10.1.2.16' , '10.1.2.17' , '10.1.2.23']
    for i in range(element):
        if(a[i][15] != ''):
            fData[k][0] = a[i][2]
            fData[k][1] = a[i][7]
            fData[k][2] = a[i][8]
            fData[k][3] = a[i][9]
            fData[k][4] = a[i][10]
            fData[k][5] = a[i][11]
            fData[k][6] = a[i][12]
            fData[k][7] = a[i][13]
            fData[k][8] = a[i][14]
            fData[k][9] = a[i][15]
            fData[k][10] = a[i][16]
            if(a[i][3] == a[i][5]):
                fData[k][11] = 0
            elif(a[i][5] == ''):
                fData[k][11] = 2
            else:
                fData[k][11] = 1
            if(a[i][4] == broad[0]):
                fData[k][12] = 1
            else:
                fData[k][12] = 0
            if(a[i][13] != ''):
                totalMessage = totalMessage + 1
                if(a[i][13] != failed[0]):
                    nfailed = nfailed + 1
            if(a[i][11] != ''):
                a[i][11] = int(a[i][11])
                tHop = tHop + a[i][11]
                nHop = nHop + 1
            check = 0
            if(t == 0):
                nNeibors[t] = a[i][3]
                nNData[t] = nNData[t] + 1
                check = 1
                t = t + 1
            else:
                for q in range(t):
                    if(a[i][3] == nNeibors[q]):
                        nNData[q] = nNData[q] + 1
                        check = 1
                        break
            if(check == 0):
                nNeibors[t] = a[i][3]
                nNData[t] = nNData[t] + 1
                t = t + 1
            val = 0
            for m in range(nAttack):
                if(a[i][3] == attacklist[m]):
                    val = 1
            if(val == 1):
                fData[k][20] = 'attack'
            else:
                fData[k][20] = 'normal'
            k = k + 1
    totrow = k
    tot = 0
    lflow = nNData[0]
    hflow = nNData[0]
    for u in range(t):
        tot = tot + nNData[u]
        if(lflow > nNData[u]):
            lflow = nNData[u]
        if(hflow < nNData[u]):
            hflow = nNData[u]
    aflow = float(tot) / (t - 1)
    aHop = float(tHop) / nHop
    trate = (float(nfailed) / totalMessage) * 100
    print(totrow,nfailed,totalMessage,nHop,tHop,aHop,t,tot,lflow,hflow,aflow,trate)
    for k in range(totrow):
        fData[k][13] = (t - 1)
        fData[k][14] = hflow
        fData[k][15] = aflow
        fData[k][16] = lflow
        fData[k][17] = aHop
        fData[k][18] = nfailed
        fData[k][19] = trate
    for i in range(totrow):
        for j in range(nFeatures):
            if(fData[i][j] == ''):
                fData[i][j] = -1
    List[:] = []
    fArray = [[0 for j in range(nFeatures)] for i in range(totrow)]
    for i in range(totrow):
        for j in range(nFeatures):
            fArray[i][j] = fData[i][j]
    Listval = list(fArray)
    #print(Listval)
    print(len(Listval))
    myFile = open('fdata24.csv', 'w')
    with myFile:
        writer = csv.writer(myFile)
        writer.writerows(Listval)
    print("Process complted..")
    pf.close()
csvread()
