/* L01: AXIOM: NUMA = Non-Uniform Memory Access.
 * L02: AXIOM: CPU Socket has local RAM (Node).
 * L03: AXIOM: Access to local Node < Remote Node latency.
 * L04: AXIOM: MAX_NUMNODES = Configurable (often 64 or 1024).
 * L05: CALCULATION (Punishment Scale):
 *      - GIVEN: Node 0 Start=0. Size=32TB (2^45 bytes).
 *      - TARGET: Node 1 Start PFN.
 *      - CALC: 32TB = 32 * 1024^4 = 35,184,372,088,832 bytes.
 *      - PFN_OFFSET: 32TB / 4096 = 8,589,934,592 (8.5 Billion).
 *      - BINARY: 10_0000_0000_0000_0000_0000_0000_0000_0000 (33 shifted bits).
 *      - CONCLUSION: Node 1 starts at PFN 0x200000000.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/module.h>
#include <linux/nodemask.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate_Coder");
MODULE_DESCRIPTION("Axiomatic NUMA Trace");

/*
 * FUNCTION: numa_zone_init
 * L06: STACK: [RBP] -> page@-8|pfn@-16|nid@-20|zone@-32.
 */
static int __init numa_zone_init(void) {
  struct page *page;
  unsigned long pfn;
  int nid;
  struct zone *zone;

  printk(KERN_INFO "NUMA: Start. CPU=%d\n", smp_processor_id());

  /*
   * STEP 1: num_online_nodes()
   * --------------------------
   * 1. WHAT: Count active sockets with RAM.
   * 2. WHY: Loop boundary.
   * 3. WHERE: Global bitmap (node_states[N_ONLINE]).
   * 4. WHO: Kernel Boot (SRAT ACPI table parsing).
   * 5. WHEN: Boot time.
   * 6. WITHOUT: CPU works, RAM works, but Scheduler/MM is inefficient.
   * 7. CALCULATION (Edge Case):
   *      - Max Nodes = 1024.
   *      - Bitmap Size = 1024 bits = 128 Bytes.
   *      - Hamming Weight (Popcount) instruction used to count 1s.
   *      - If machine has holes (Nodes 0, 2, 8 present).
   *      - Count = 3. Max ID = 8.
   */
  printk(KERN_INFO "NUMA: Nodes Online=%d\n", num_online_nodes());

  /*
   * STEP 2: Loop Nodes
   * ------------------
   * 1. WHAT: Iterate 0..MAX_NUMNODES.
   * 2. WHY: Inspect topology.
   * 3. WHERE: pgdat_list (or similar).
   * 4. CALCULATION (Harder):
   *      - Accessing NODE_DATA(nid).
   *      - On x86: Array of Pointers to pg_data_t.
   *      - Size of pg_data_t ≈ 140KB (Huge struct).
   *      - If 64 Nodes: 64 * 140KB = 8.75MB just for metadata structures!
   */
  for_each_online_node(nid) {
    pg_data_t *pgdat = NODE_DATA(nid);
    printk(KERN_INFO "NUMA: Node[%d] StartPFN=%lu Spanned=%lu\n", nid,
           pgdat->node_start_pfn, pgdat->node_spanned_pages);
  }

  /*
   * STEP 3: alloc_page(GFP_KERNEL)
   * ------------------------------
   * 1. WHAT: Get 1 Page from *Local* Node.
   * 2. WHY: Performance (minimize interconnect traffic).
   * 3. WHERE: Current CPU's Node -> Zone Normal.
   * 4. WHO: Buddy.
   * 5. WITHOUT: Fallback to Remote Node (slower).
   * 6. CALCULATION (Punishment - Latency):
   *      - Local Access: ~60ns.
   *      - Remote Access (1 hop): ~100ns (QPI/UPI Link).
   *      - Remote Access (2 hops): ~140ns.
   *      - If Loop 1M times: 1M * 40ns diff = 40ms wasted time.
   */
  page = alloc_page(GFP_KERNEL);
  if (!page)
    return -ENOMEM;

  /*
   * STEP 4: page_to_nid(page)
   * -------------------------
   * 1. WHAT: Extract Node ID from page flags.
   * 2. WHY: Verify locality.
   * 3. WHERE: struct page -> flags (Upper bits).
   * 4. CALCULATION (Bitwise Punishment):
   *      - Flags = 64 bits.
   *      - Layout: [Section|Node|Zone|Flags].
   *      - If Node is bits 50-56 (example).
   *      - Mask: 0x3F << 50.
   *      - Shift: (Flags >> 50) & 0x3F.
   *      - If Flags = 0x8040...0000. (Bit 51 set).
   *      - NID = 2.
   */
  nid = page_to_nid(page);
  zone = page_zone(page);
  pfn = page_to_pfn(page);

  printk(KERN_INFO "NUMA: Alloc Page@PFN %lu -> Node %d Zone %s\n", pfn, nid,
         zone->name);

  __free_page(page);
  return 0;
}

static void __exit numa_zone_exit(void) { printk(KERN_INFO "NUMA: Exit.\n"); }

module_init(numa_zone_init);
module_exit(numa_zone_exit);

/*
 * === DENSE AXIOMATIC TRACE: numa_zone_init ===
 * DRAW[Memory:Stack@RBP→page|pfn|nid|zone]@Boot→CALL[num_online_nodes()]→READ[Bitmap]→RETURN[1]→LOG[Nodes=1]→DRAW[Loop:nid=0]→READ[NODE_DATA(0)]→PTR_DEREF[0xffff...]→READ[pgdat->start_pfn]→LOG[Start=0]→CALL[alloc_page(GFP_KERNEL)]→SEARCH[LocalNode=0]→SEARCH[ZoneNormal]→POP[FreeList]→RETURN[Page@PFN10500]→STORE[page]→CALL[page_to_nid(Page)]→BIT_OP[Flags>>NodeBiShift]→RETURN[0]→STORE[nid=0]→CALL[page_zone(Page)]→BIT_OP[Flags>>ZoneShift]→RETURN[ZoneNormalPtr]→CALL[page_to_pfn]→CALC[(Ptr-Base)/64]→RETURN[10500]→LOG[PFN10500:Node0:Normal]→CALL[__free_page]→RETURN[Success]
 */
