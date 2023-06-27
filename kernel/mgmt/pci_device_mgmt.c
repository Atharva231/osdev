#include <stdint.h>
#include <stdbool.h>

struct pci_device_list{
    uint16_t dev_id;
    uint16_t ven_id;
    uint16_t class_id;
    uint16_t subclass_id;
    struct pci_device_list* next;
}* pci_device;

struct pci_device_list* pci_init(){
    pci_device=(struct pci_device_list*)mem_alloc(sizeof(struct pci_device_list));
    struct pci_device_list* pci_temp;
    pci_temp=pci_device;
    uint8_t i=0;
    while(true){
        pci_temp->dev_id=PCI_Device_ID(0, i, 0);
        pci_temp->ven_id=PCI_Vendor_ID(0, i, 0);
        pci_temp->class_id=PCI_Class_ID(0, i, 0);
        pci_temp->subclass_id=PCI_Subclass_ID(0, i, 0);
        i+=1;
        if(PCI_Class_ID(0, i, 0)==0xFF){
            pci_temp->next=0;
            break;
        }
        pci_temp->next=(struct pci_device_list*)mem_alloc(sizeof(struct pci_device_list));
        pci_temp=pci_temp->next;
    }
    return pci_device;
}