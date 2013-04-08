<?
// Usage: tcpdump -qnnttl [options] | php iptraf.php

class Parser {
	protected $lastTime;
	protected $stats = array();

	function trafficCmp($a, $b) {
		return $b["b_down"] - $a["b_down"];
	}

	function printStats() {
		if (empty($this->stats))
			return;
		uasort($this->stats["IP"], array($this, "trafficCmp"));
		echo "\033[2J\033[1;1H";
		printf("%16s %16s %16s %16s %16s\n\n",
				"IP",
				"RX kbytes/s",
				"RX kpackets/s",
				"TX kbytes/s",
				"TX kpackets/s");
		foreach ($this->stats["IP"] as $ip => $data) {
			printf("%16s %16.2f %16.2f %16.2f %16.2f\n",
					$ip,
					$data["b_down"] / 1024,
					$data["p_down"] / 1000,
					$data["b_up"] / 1024,
					$data["p_up"] / 1000);
		}
	}

	function match($ip) {
		return substr($ip, 0, 5) == "10.1.";
	}

	function parse($line) {
		list($time, $l3proto, $data) = explode(' ', $line, 3);
		$time = explode('.', $time);

		if ($time[0] != $this->lastTime) {
			$this->printStats();
			$this->lastTime = $time[0];
			$this->stats = array();
		}
		if (!isset($this->stats[$l3proto]))
			$this->stats[$l3proto] = array();
		$stats =& $this->stats[$l3proto];

		switch ($l3proto) {
		case 'IP':
			list($ip, $data) = explode(': ', $data);
			$ip = explode(' > ', $ip);
			$ip[0] = implode('.', explode('.', $ip[0], -1));
			$ip[1] = implode('.', explode('.', $ip[1], -1));
			if ($this->match($ip[0])) {
				$ip = $ip[0];
				$dir = "up";
			} elseif ($this->match($ip[1])) {
				$ip = $ip[1];
				$dir = "down";
			} else
				break;

			if (substr($data, 0, 4) == "tcp ") {
				$data = explode(' ', $data);
				$len = $data[1];
			} elseif (substr($data, 0, 4) == "UDP,") {
				$data = explode(' ', $data);
				$len = $data[2];
			} else {
				//echo "Ignoring unknown IP proto: " . $data;
				break;
			}

			if (!isset($stats[$ip]))
				$stats[$ip] = array(
						"b_up" => 0,
						"b_down" => 0,
						"p_up" => 0,
						"p_down" => 0
						);
			$stats[$ip]["b_" . $dir] += $len;
			$stats[$ip]["p_" . $dir]++;
			break;
		default:
			//echo "Ignoring unknown proto '" . $l3proto . "'\n";
		}
	}
}

$fp = fopen("php://stdin", "r");
$parser = new Parser();
while ($line = fgets($fp))
	$parser->parse($line);
fclose ($fp);

?>
