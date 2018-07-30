class EvidenceList:
    limit = 35

    class Evidence:
        def __init__(self, name, desc, image, pos):
            self.name = name
            self.desc = desc
            self.image = image
            self.public = False
            self.pos = pos

        def set_name(self, name):
            self.name = name

        def set_desc(self, desc):
            self.desc = desc

        def set_image(self, image):
            self.image = image

        def to_string(self):
            sequence = (self.name, self.desc, self.image)
            return '&'.join(sequence)
            
    def __init__(self):
        self.evidences = []
        self.poses = {'def':['def', 'hld'], 'pro':['pro', 'hlp'], 'wit':['wit'], 'hlp':['hlp', 'pro'], 'hld':['hld', 'def'], 'jud':['jud'], 'all':['hlp', 'hld', 'wit', 'jud', 'pro', 'def', ''], 'pos':[]}
        
    def login(self, client):
        if client.area.evidence_mod == 'FFA':
            pass
        if client.area.evidence_mod == 'Mods':
            if not client.is_cm:
                return False
        if client.area.evidence_mod == 'CM':
            if not client.is_cm and not client.is_mod:
                return False
        if client.area.evidence_mod == 'HiddenCM':
            if not client.is_cm and not client.is_mod:
                return False
        return True

    def correct_format(self, client, desc):
        if client.area.evidence_mod != 'HiddenCM':
            return True
        else:
            #correct format: <owner = pos>\ndesc
            if desc[:9] == '<owner = ' and desc[9:12] in self.poses and desc[12:14] == '>\n':
                return True
            return False
            
        
    def add_evidence(self, client, name, description, image, pos = 'all'):
        if self.login(client):
            if client.area.evidence_mod == 'HiddenCM':
                pos = 'pos'
            if len(self.evidences) >= self.limit:
                client.send_host_message('You can\'t have more than {} evidence items at a time.'.format(self.limit))
            else:
                self.evidences.append(self.Evidence(name, description, image, pos))
        
    def evidence_swap(self, client, id1, id2):
        if self.login(client):
            self.evidences[id1], self.evidences[id2] = self.evidences[id2], self.evidences[id1]
            
    def create_evi_list(self, client):
        evi_list = []
        nums_list = [0]
        for i in range(len(self.evidences)):
            if client.area.evidence_mod == 'HiddenCM' and self.login(client):
                nums_list.append(i + 1)
                evi = self.evidences[i]
                evi_list.append(self.Evidence(evi.name, '<owner = {}>\n{}'.format(evi.pos, evi.desc), evi.image, evi.pos).to_string())
            elif client.pos in self.poses[self.evidences[i].pos]:
                nums_list.append(i + 1)
                evi_list.append(self.evidences[i].to_string())
        return nums_list, evi_list
    
    def del_evidence(self, client, id):
        if self.login(client):
            self.evidences.pop(id)
        
    def edit_evidence(self, client, id, arg):
        if self.login(client):
            if client.area.evidence_mod == 'HiddenCM' and self.correct_format(client, arg[1]):
                self.evidences[id] = self.Evidence(arg[0], arg[1][14:], arg[2], arg[1][9:12])
                return
            if client.area.evidence_mod == 'HiddenCM':
                client.send_host_message('You entered a wrong pos.')
                return
            self.evidences[id] = self.Evidence(arg[0], arg[1], arg[2], arg[3])