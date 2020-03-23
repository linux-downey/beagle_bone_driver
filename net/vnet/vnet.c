#include <linux/init.h>             
#include <linux/module.h>          
#include <linux/kernel.h>   
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/gpio.h>
#include <linux/if_link.h>

struct ndev_info *info;
MODULE_LICENSE("GPL");              

struct ndev_info{
	struct net_device *vnet_dev;
};

static void print_info(struct net_device *dev)
{
	printk("dev->name = %s\n",dev->name);
	printk("dev->mem_start = %lu\n",dev->mem_start);
	printk("dev->mem_end = %lu\n",dev->mem_end);
	printk("dev->base_addr = %lu\n",dev->base_addr);
	printk("dev->irq = %lu\n",dev->irq);		
}

void print_buf(unsigned char *buf,unsigned int len)
{
	int i = 0;
	for(i=0;i<len;i++)
	{
		printk("%.2x.",buf[i]);
	}
	printk("\n");
}
static void print_skbuff_info(struct sk_buff *skb)
{
	

	printk("skb = 0x%x\n",(unsigned long)skb);
	printk("sizeof skb = %lu\n",sizeof(*skb));

	printk("skb->len = %lu\n",skb->len);
	printk("skb->data_len = %lu\n",skb->data_len);
	printk("skb->mac_len = %lu\n",skb->mac_len);
	printk("skb->hdr_len = %lu\n",skb->hdr_len);


	//printk("skb->hdr_len = %lu\n",skb->hdr_len);
	//printk("skb->hdr_len = %lu\n",skb->hdr_len);
	//printk("skb->hdr_len = %lu\n",skb->hdr_len);

	printk("skb->protocol = %x\n",skb->protocol);

	printk("skb->network_header = 0x%x\n",skb->network_header);

	printk("skb->transport_header = 0x%x\n",skb->transport_header);
	printk("skb->mac_header = 0x%x\n",skb->mac_header);

	printk("skb->head = %x\n",skb->head);
	printk("skb->end = %x\n",skb->end);
	printk("skb->data = %x\n",skb->data);
	printk("skb->tail = %x\n",skb->tail);

	printk("skb->truesize = %lu\n",skb->truesize);

	print_buf((unsigned char*)(skb->head+skb->mac_header),14);
	
	if(GFP_ATOMIC & (__GFP_DIRECT_RECLAIM | GFP_DMA)){
		printk("true!\n");
	}else{
		printk("false!\n");
	}
	printk("SKB_WITH_OVERHEAD(PAGE_SIZE) = %lu\n",SKB_WITH_OVERHEAD(PAGE_SIZE));
	if(ETH_P_IP == skb->protocol)
	{

	}
}

static int vnet_open(struct net_device *dev)
{
	printk("Enter vnet open!!\n");
	print_info(dev);
	return 0;
}


static int vnet_stop(struct net_device *dev)
{
	printk("Enter vnet stop!!\n");
	print_info(dev);
	return 0;
}

static int vnet_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	//struct vnet_info* info = netdev_priv(dev);
	printk("vnet start xmit!!!!\n");
	
	print_skbuff_info(skb);

	return 0;
}

static struct rtnl_link_stats64 *vnet_get_stats64(struct net_device *dev,struct rtnl_link_stats64 *storage)
{
	storage->tx_packets = 5;
	return storage;
}

static struct net_device_stats* vnet_get_stats(struct net_device *dev)
{
	dev->stats.tx_packets = 10;
	return &dev->stats;
}

static const struct net_device_ops vnetdev_ops = {
	.ndo_open		= vnet_open,
	.ndo_stop		= vnet_stop,
	.ndo_start_xmit		= vnet_start_xmit,
	//ndo_get_stats64        = vnet_get_stats64,
	//.ndo_get_stats          = vnet_get_stats,
	
	// .ndo_tx_timeout		= dm9000_timeout,
	// .ndo_set_rx_mode	= dm9000_hash_table,
	// .ndo_do_ioctl		= dm9000_ioctl,
	// .ndo_set_features	= dm9000_set_features,
	// .ndo_validate_addr	= eth_validate_addr,
	// .ndo_set_mac_address	= eth_mac_addr,
};


static int vnet_header(struct sk_buff *skb, struct net_device *dev,
			       unsigned short type, const void *daddr,
			       const void *saddr, unsigned len)
{
	printk("Enter vnet header create!!\n");
	return 0;
}

static int vnet_parse(const struct sk_buff *skb, unsigned char *haddr)
{
	printk("Enter vnet header parse!!\n");
	return 0;
}

static int vnet_cache(const struct neighbour *neigh, struct hh_cache *hh, __be16 type)
{
	printk("Enter vnet header cache!!\n");
	return 0;
}

static void vnet_update(struct hh_cache *hh,
				const struct net_device *dev,
				const unsigned char *haddr)
{
	printk("Enter vnet header update!!\n");
	return;
}

static const struct header_ops vnethead_ops = {
	.create  	= vnet_header,
	.parse		= vnet_parse,
	.cache		= vnet_cache,
	.cache_update	= vnet_update,
};

void vnet_setup(struct net_device *dev)
{
	dev->netdev_ops = &vnetdev_ops;
	dev->header_ops = &vnethead_ops;
}

static int __init vnet_init(void)
{
	struct net_device *vnet_dev;
	
	int ret;
	printk(KERN_DEBUG "vnet init!!!\n");

	vnet_dev = alloc_etherdev(sizeof(struct ndev_info));
	if(NULL == vnet_dev){
		goto err1;
	}
	//vnet_dev = alloc_netdev(sizeof(struct ndev_info),"vnet%d",NET_NAME_UNKNOWN,vnet_setup);
	//if(NULL == vnet_dev){
	//	goto err1;
	//}
	info = netdev_priv(vnet_dev);
	vnet_dev->netdev_ops = &vnetdev_ops;

	info->vnet_dev = vnet_dev;
	
	ret = register_netdev(vnet_dev);
	if(0 == ret){
		printk("register netdev ok!\n");
	}
	return 0;

	err1:
		printk("failed to alloc etherdev!\n");
		return -ENOMEM;
	
}

static void __exit vnet_exit(void)
{
	
	unregister_netdev(info->vnet_dev);
	free_netdev(info->vnet_dev);
	printk(KERN_DEBUG "vnet exit!!!\n");
}

module_init(vnet_init);
module_exit(vnet_exit);
